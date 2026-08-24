# Lab 2_3 — Group A__

## 1. Tool and install route — [3]

| | |
|---|---|
| Agent used for run 2 | Antigravity IDE (Gemini 3.1 Pro) |
| `ubiquitous-language` install route | project `.agents/skills` |
| `refactoring/` pack install route | project `.agents/skills` |

If anything would not install, say what failed here. The paste fallback is a documented
route and costs no marks.

---

## 2. What I changed in the glossary — [4]

The generated file is at `lab2_3/UBIQUITOUS_LANGUAGE.md`. Three or four lines here on what
you corrected and why: terms it invented, definitions it got wrong, ambiguities it missed.

I reviewed the auto-generated glossary. The agent correctly identified `Snake`, `Food`, `Position`, `Game`, `Score`, and `High Score`. I clarified that `Board` is an implicit concept and marked it as a 'Code drift' because there is no explicit struct/class for it (only `WIDTH`/`HEIGHT`/`drawBoard`). I removed generic C++ terminology that the model might otherwise include and focused entirely on the domain terms.

---

## 3. Smell delta — [6]

Reports: `lab2_3/audits/main.md` (the code as you received it) and `lab2_3/audits/lab1-head.md`
(after your Lab-1 PR).

| | count | representative site (`file:line`) |
|---|---|---|
| Smells my Lab-1 PR **introduced** | 3 | `snake.cpp:189` (Duplicate Code in update) |
| Smells my Lab-1 PR **left untouched** | 2 | `snake.cpp:88` (Large Class) |
| Smells my Lab-1 PR **removed** | 0 | — |

The third row will probably be zero. Leave it in.

---

## 4. Rejected candidates — [6]

At least three things the agent reported that are *not* real findings on this codebase.

| smell reported | `file:line` | why it does not hold |
|---|---|---|
| Switch Statements (Long if-else) | `snake.cpp:160` | Only maps 8 keys; using a full Strategy/Command pattern for 8 keys in a small game is massive speculative overengineering. |
| Data Class | `snake.cpp:19` | `Position` is meant to be a simple Plain Old Data struct for (x, y) coordinates. Adding behavior to it would overcomplicate it. |
| Feature Envy | `snake.cpp:82` | `Food::spawn` needs to know where the snake is to avoid spawning inside it. Moving this logic to `Snake` would break SRP. |

---

## 5. Commit map — [7]

Run `lab2_3/check-lab2_3.sh` and paste the table it prints.

| # | sha | subject | what it is |
|---|---|---|---|
| 1 | 6067736 | Commit 1: Add ubiquitous language glossary | glossary |
| 2 | 4c6c4bf | Commit 2: Add main.md code smells audit | smell report |
| 3 | a70fc0f | Commit 3: Refactor to support multiple snakes using vector | **the refactor, alone** |
| 4 | 5c4f422 | Commit 4: Add second player | **the feature, alone** |

---

## 6. Two-run measurement — [4]

Run 1 is your Lab-1 branch — the numbers you already reported. Run 2 is commit 4 alone.

| | Run 1 (Lab 1) | Run 2 (commit 4) |
|---|---|---|
| Smells introduced | 3 | 0 |
| Lines changed, `git diff --shortstat -w` | 82 | 40 |
| Lines changed, **raw** (no `-w`) | 528 | 46 |
| Functions reached | 4 | 4 |
| Prompts to working code | 3 | 1 |
| Wall-clock time | 15m | 5m |

Commit 3 (the refactor) on its own: 84 lines `-w`, 94 raw.

`check-lab2_3.sh` prints the four line-count numbers for run 2. Use them — they are measured
the same way for every group, which is what makes the class comparison mean anything.

---

## 7. Analysis Q1–Q2 — [5]

**Q1. Which smell did commit 3 actually fix?** Name it from your section 3 report. What was
expensive before, what does it cost now.

Commit 3 fixed the "Shotgun Surgery" and "Duplicate Code" smells in `Game`. Before, adding a player required duplicating the snake's state fields and copying entire blocks of collision and movement logic. Now, with a `vector<Snake>`, adding a player only costs resizing the vector and adding specific key mappings, without duplicating logic.

**Q2. Compare commit 4 to your Lab-1 diff.** Same feature, same codebase. What changed in
the cost and what did not? If it got worse, say so and explain — that marks the same.

In Lab 1, the feature took 82 lines (-w) of repetitive code. In commit 4, the feature only required 40 lines (-w). The cost (lines changed) dropped by half because the refactored design natively supported the new requirement. We didn't have to duplicate the collision logic. What did not change was the need to manually add WASD key checks in `processInput`, which still requires repetitive conditional statements.

---

## 8. Analysis Q3–Q4 — [5]

**Q3. Go back through your Lab-1 `LLM-LOG.md`. Did the assistant ever suggest restructuring
before adding the feature?** Quote it if it did. If it did not, what would have had to be
different in your prompt?

No, the assistant did not suggest restructuring. It simply solved the prompt in the most direct way possible by copying and pasting the existing variables and logic. To get a better design, I would have had to explicitly ask it to "refactor the snake to use a dynamic array before adding the second player" or ask it to "prioritize avoiding code duplication."

**Q4. How do you know commit 3 did not change behaviour?** Answer honestly. Most of you will
find that you do not know. Say that plainly if it is true, and describe what you would have
needed in order to actually know.

I don't know for certain. Since there are no automated unit tests for `snake.cpp` and it heavily couples game logic with POSIX terminal I/O, I can only verify it manually by playing the game. To actually know, I would have needed an automated test suite that decoupled the game logic from the rendering, allowing me to simulate game ticks and verify state assertions automatically.

---

Sections 7 and 8 together: **500 words maximum.**

## If you did not finish

Say so here and lose no marks. What you got to, where it broke, what you tried.

N/A
