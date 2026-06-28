# Animatronic AI Walkthrough (`ai.hpp` / `ai.cpp`)

Notes on how the animatronic AI layer works in console-fnaf — syntax, inheritance, and game logic.

---

## How `getCooldownDuration` is used

Flow inside `update()`:

1. Each frame, `cooldown` counts down.
2. If `cooldown > 0`, the AI skips the random "should I advance?" roll.
3. When the roll succeeds and `advance(game)` runs, cooldown is **set** from `getCooldownDuration()`:

```cpp
if (r < baseChance)
{
    advance(game);
    cooldown = getCooldownDuration();
}
```

The formula (in `ai.cpp`):

```cpp
float AnimatronicAI::getCooldownDuration() const {
    return 1.0f + (1.0f - aggro) * 2.0f;
}
```

| `aggro` | cooldown after a move |
|--------:|----------------------:|
| 0.0     | 3.0 seconds           |
| 1.0     | 1.0 second            |

Calm animatronics wait longer between moves; angry ones move more often. Neither `FreddoAI` nor `ChicoAI` overrides this, so they both use the base formula and each keeps its **own** `cooldown` member (inherited from `AnimatronicAI`).

**Note:** when blocked at the door and retreating, cooldown is hard-coded to `2.0f`, not from `getCooldownDuration()`.

---

## The four floats at the top — member variables

```cpp
float aggro = 0.0f;
float cooldown = 0.0f;
float decisionAcc = 0.0f;
float blockedTimer = 0.0f;
```

These are **data members** (instance variables). Each `FreddoAI` / `ChicoAI` object gets its own copy.

- `aggro` — how aggressive (0→1), grows over time
- `cooldown` — seconds until next move attempt
- `decisionAcc` — accumulates time; AI decides once per second
- `blockedTimer` — how long stuck at a closed door before retreating

`= 0.0f` is **default member initialization**: every new object starts at 0 without writing that in a constructor.

---

## `virtual ~AnimatronicAI() = default` — destructor basics

A **destructor** runs when an object is destroyed (goes out of scope, `delete`d, etc.). It is for cleanup (free memory, close files, etc.).

### `= default`

Tells the compiler: "generate the normal destructor for me." Use it when you do not need custom cleanup. Here there is no manual `new`/`delete`, so default is fine.

Write your own destructor when you manage resources the compiler cannot handle.

### `virtual`

Needed because `FreddoAI` and `ChicoAI` **inherit** from `AnimatronicAI`. If you ever did:

```cpp
AnimatronicAI* bot = new FreddoAI();
delete bot;  // virtual destructor → FreddoAI destroyed correctly
```

Without `virtual`, deleting through a base pointer might only run the base destructor — undefined behavior with inheritance.

**Rule of thumb:** if a class has any `virtual` functions and is meant to be inherited, make the destructor `virtual` (often `= default`).

---

## Those "float things" — member functions, not variables

```cpp
virtual float getAggroRate(const GameState &game) const;
virtual float getRetreatThreshold() const;
virtual float getCooldownDuration() const;
```

These are **member functions** (methods). `float` before the name is the **return type**, not a variable type declared after the fact.

Read it left to right:

```
return-type   function-name(parameters)   qualifiers;
float         getCooldownDuration()       const;
```

### `const` after the function (trailing const)

Means a **const member function**: this method promises not to modify the object's own members (`aggro`, `cooldown`, etc.).

```cpp
float getCooldownDuration() const;  // can read aggro, must not change it
void advance(GameState &game);      // could change aggro, cooldown, game, etc.
```

`isBlocked` / `atDoor` / `getCooldownDuration` only **read** state → `const` on the function.

`advance` / `retreat` **change** `game` (positions) → not `const` on the function (and they take non-const `GameState &`).

### `= 0` on other methods (pure virtual)

```cpp
virtual bool isBlocked(const GameState &game) const = 0;
virtual void advance(GameState &game) = 0;
virtual void retreat(GameState &game) = 0;
virtual bool atDoor(const GameState &game) const = 0;
```

`= 0` means **pure virtual**: no default implementation; `FreddoAI` and `ChicoAI` **must** implement these. You cannot create a plain `AnimatronicAI` object — it is an abstract base class.

---

## `const GameState &game` — parameter syntax

Three pieces:

| Part        | Meaning                                              |
|-------------|------------------------------------------------------|
| `GameState` | Type of the parameter                                |
| `&`         | **Reference** — alias to the original object, no copy |
| `const`     | Read-only through this parameter                     |

### Variations

```cpp
void foo(GameState game);        // pass by value — full copy
void foo(GameState &game);       // reference — can modify original
void foo(const GameState &game); // reference — read-only, efficient
```

In this project:

- `isBlocked(const GameState &game)` — only reads `game.leftDoor`, etc.
- `advance(GameState &game)` — modifies `game.freddoPos++`

Without `&`: every call copies the whole `GameState` (wasteful for a struct passed every frame).

Without `const`: the function is allowed to modify `game`; the compiler will not stop accidental writes in "query" functions.

---

## Why `update` has `GameState &game` without `const`

```cpp
void update(GameState &game, float deltaSeconds);
```

`update` **mutates both**:

1. **This AI object** — `aggro`, `cooldown`, `decisionAcc`, `blockedTimer`
2. **`game`** — via `advance(game)` and `retreat(game)`, which change positions

So `game` must be non-const. If it were `const GameState &`, lines like `game.freddoPos++` would not compile.

`deltaSeconds` is passed by value (`float`) — cheap to copy, and the function does not need to change the caller's variable.

---

## `FreddoAI : AnimatronicAI` — inheritance

```cpp
struct FreddoAI : AnimatronicAI
{
    bool isBlocked(const GameState &game) const override;
    void advance(GameState &game) override;
    void retreat(GameState &game) override;
    bool atDoor(const GameState &game) const override;
};
```

The colon means **inherits from** (for `struct`, inheritance is public by default).

So `FreddoAI` **is an** `AnimatronicAI` plus Freddy-specific behavior:

- Gets all four floats + `update()` + `getCooldownDuration()` etc.
- Must implement the four pure virtual methods
- `override` = "this replaces the base virtual function" (compiler checks the signature matches)

In `main.cpp`:

```cpp
freddo.update(game, deltaSeconds);
chico.update(game, deltaSeconds);
```

Same `update()` loop for both; polymorphism picks the right `advance` / `retreat` / `atDoor` / `isBlocked` per type.

---

## Why `isBlocked` / `atDoor` have `const`, but `advance` / `retreat` do not

Two separate `const` concepts:

### 1. `const` on the member function (after `)`)

Applies to **`this`** (the AI object):

```cpp
bool atDoor(const GameState &game) const;  // won't change aggro, cooldown, ...
void advance(GameState &game);             // may change this object's members (allowed)
```

Freddy's `advance` only touches `game`, but the base declares it non-const, so derived matches that.

### 2. `const` on the parameter

Applies to **`game`**:

```cpp
bool isBlocked(const GameState &game) const;  // read game only
void advance(GameState &game);                // modify game (positions)
```

| Method      | `const` on `this`? | `const` on `game`? | Why                              |
|-------------|:------------------:|:------------------:|----------------------------------|
| `isBlocked` | yes                | yes                | query door + query AI state      |
| `atDoor`    | yes                | yes                | query position                   |
| `advance`   | no                 | no                 | moves animatronic in `game`      |
| `retreat`   | no                 | no                 | moves animatronic back in `game` |

---

## Big picture: template method pattern

```
AnimatronicAI (base)
├── Shared state: aggro, cooldown, timers
├── Shared logic: update() — aggro, cooldown, random roll, retreat when blocked
├── Hooks with defaults: getAggroRate, getRetreatThreshold, getCooldownDuration
└── Hooks you must customize: isBlocked, advance, retreat, atDoor

FreddoAI  → left door, freddoPos, door at 6
ChicoAI   → right door, chicoPos, door at 7
```

One game loop calls `update()` on each animatronic; inheritance supplies **what is different** (which door, which position field, door threshold) while the base supplies **what is the same** (aggro curve, cooldown pacing, once-per-second decisions).

---

## Quick syntax cheat sheet

```cpp
struct Base {
    float x = 0.0f;                              // member variable
    virtual ~Base() = default;                   // polymorphic cleanup
    virtual void mustImpl() = 0;                 // pure virtual — subclass required
    virtual float hook() const;                  // optional override; const = won't change *this
    void sharedLogic(Thing &t, float dt);        // non-virtual shared code
};

struct Derived : Base {
    void mustImpl() override;                    // implements pure virtual
};
```

---

## Related files

- `src/ai.hpp` — declarations
- `src/ai.cpp` — `update()` and hook implementations
- `src/game_state.hpp` — `GameState` struct passed into AI methods
- `src/main.cpp` — game loop calls `freddo.update()` and `chico.update()`
