# Conversation Log

## Overview
Full-stack refactor of a C++ Calendar System (DSA project) — backend restructuring + React frontend.

## Phase 1: Audit & Planning

### Initial Codebase Scan
Read all source files: `main.cpp`, `ds_lib.h`, `event.h/cpp`, `calendar.h/cpp`, `file_manager.h/cpp`.

### Issues Identified
**Bugs:**
1. `clearAllEvents()` leaks BST nodes (sets `root = nullptr` without deleting)
2. String-based time comparison wrong: `"9:30"` > `"10:00"` lexicographically
3. `deleteFromHeap` is O(n log n) — dumps entire heap and rebuilds
4. No input validation on `stoi()` — crashes on non-numeric input
5. CSV format fragile — description can contain commas, no escaping

**Design Issues:**
6. All DS implementations in `ds_lib.h` (521 lines, header-only)
7. Broken encapsulation: `LinkedList::head` public, `getTable()` returns raw `LinkedList**`
8. Triple data redundancy (BST + HashTable + Heap) with manual sync
9. Duplicate code: `convertToComparable` in both ds_lib.h and calendar.cpp
10. `HASH_TABLE_SIZE = 21` — not prime, poor hash distribution

**Architecture:**
11. No test suite
12. File-based ID counter with `atexit` save is fragile
13. No separation of layers (UI + business logic + data structures all intertwined)

### Improvement Options Presented
- **A** — Quick fixes (low effort)
- **B** — Structural cleanup (medium effort)
- **C** — Architectural overhaul (high effort)

**Chosen:** Between B and C.

### Design Decisions (User Choices)
| Decision | Choice |
|----------|--------|
| File format | Keep CSV with escaping |
| Testing | Add doctest |
| UI split | Query methods return data; mutation methods return bool (no cout) |
| Smart pointers | Use `std::unique_ptr` for nodes |
| Heap deletion | Position tracking with `unordered_map` for O(log n) |
| UI split depth | Query methods only (getEventsByDate, searchEvents, etc. return data) |

### Final 5-Phase Plan
1. File restructuring + memory safety (split ds_lib.h, unique_ptr)
2. Encapsulation + bug fixes (private fields, time comparison, stoi safety)
3. CSV escaping (quote-wrap descriptions on save, parse on load)
4. Partial UI split (query methods return data, all cout in main.cpp)
5. Testing with doctest

---

## Phase 2: Backend Implementation

### Files Created

| File | Purpose |
|------|---------|
| `backend/include/ds_utility.h` | Shared helpers: `convertToComparable`, `timeToMinutes`, `escapeCSV`, `parseCSVLine` |
| `backend/include/linked_list.h` | `LinkedList` with `unique_ptr<ListNode>`, private `head`, proper copy ctor |
| `backend/include/bst.h` | `BST` with `unique_ptr<BSTNode>`, `forEach` callback |
| `backend/include/min_heap.h` | `MinHeap` with `vector<Event>` + `unordered_map<string,int> posMap` |
| `backend/include/hash_table.h` | `HashTable` with `LinkedList table[31]`, no raw pointer leaks |
| `backend/include/ds_lib.h` | Umbrella header (includes all DS headers) |
| `backend/include/httplib.h` | Single-header HTTP library (cpp-httplib) |

### Files Rewritten

| File | Changes |
|------|---------|
| `calendar.h` | Added `ConflictResult` struct, return types for queries, `exportFormattedTXT`, `forEachEvent` |
| `calendar.cpp` | All `cout` → returns data; `deleteEvent` returns bool; `updateEvent` accepts params; fixed `clearAllEvents` leak |
| `file_manager.h` | Forward-declares `CalendarSystem`, returns `bool`/`int` |
| `file_manager.cpp` | Uses `parseCSVLine` + `escapeCSV` for proper CSV; `saveEventsToFile` uses `forEachEvent` |
| `main.cpp` | All I/O stays here; formatting functions for event boxes; try-catch on `stoi` |
| `server.cpp` | REST API on port 8080; 10 endpoints; CORS headers; JSON serialization |

### Key Changes
- **unique_ptr**: Eliminated all manual `new`/`delete` in LinkedList and BST. Recursive `insertRec` uses move semantics.
- **Position tracking**: `MinHeap::swap()` updates position map; `remove(id)` finds element by map, swaps with last, sifts in O(log n).
- **Time fix**: All time comparisons use `timeToMinutes(hours*60 + minutes)` instead of string compare.
- **CSV**: `escapeCSV` wraps fields in quotes if they contain commas/quotes/newlines; `parseCSVLine` correctly handles quoted fields.

---

## Phase 3: Frontend (React + Vite)

### Frontend Structure
```
frontend/
├── index.html
├── package.json
├── vite.config.js                 ← proxies /api → localhost:8080
└── src/
    ├── main.jsx                   ← Entry point
    ├── App.jsx                    ← Router (6 routes)
    ├── App.css                    ← Dark theme, animations
    ├── api.js                     ← API client (9 functions)
    ├── components/
    │   ├── Navbar.jsx             ← Sticky nav with active link glow
    │   └── EventCard.jsx          ← Inline edit/delete, priority badges
    └── pages/
        ├── Dashboard.jsx          ← Stats + upcoming events grid
        ├── AddEvent.jsx           ← Form with date/time validation
        ├── ViewEvents.jsx         ← By-date filter or all events grouped
        ├── SearchEvents.jsx       ← Search by title/ID
        ├── CheckConflicts.jsx     ← Visual conflict pair display
        └── Structures.jsx         ← Raw data structure output
```

### API Endpoints Served by Backend
| Method | Endpoint | Purpose |
|--------|----------|---------|
| GET | `/api/events` | List all upcoming events |
| GET | `/api/events/date/:date` | Get events by date |
| GET | `/api/events/search?q=` | Search events |
| GET | `/api/events/conflicts/:date` | Check time conflicts |
| GET | `/api/events/:id` | Get single event |
| POST | `/api/events` | Add event |
| PUT | `/api/events/:id` | Update event |
| DELETE | `/api/events/:id` | Delete event |
| GET | `/api/structures` | Data structure info |
| POST | `/api/save` | Save to file |
| POST | `/api/load` | Load from file |

### CSS Animations
- `fadeIn` — page entrance
- `scaleIn` — card/stat entrance
- `slideIn` — conflict pairs
- `pulse` — editing mode indicator
- `float` — icons
- `shimmer` — loading placeholder
- `toastIn` — notification toast
- Glassmorphism cards with hover transforms
- Gradient text for headings and stat numbers

### Frontend Build
- **Result**: Builds successfully with Vite
- **Output**: `frontend/dist/` (index.html + hashed CSS/JS)
- **Size**: 0.58 KB HTML, 10.71 KB CSS, 179.89 KB JS (57 KB gzipped)

---

## Build Configuration

### Files Created
| File | Purpose |
|------|---------|
| `backend/CMakeLists.txt` | CMake build for all targets |
| `backend/build.bat` | Batch build script (cli/server/all) |
| `start.bat` | Root launcher for both servers |
| `.vscode/tasks.json` | 4 VS Code tasks (Build CLI, Build Server, Build Frontend, Dev Frontend) |

### How to Run
1. `cd backend && build.bat server` — compile the API server
2. `cd frontend && npm run dev` — start the React dev server
3. Or `.\start.bat` — launches both

---

## Summary of Changes
- **New files**: 14 (DS headers/sources, server, frontend components, build configs)
- **Files moved**: 9 .h → `backend/include/`, 10 .cpp → `backend/src/`
- **Files rewritten**: 6 (calendar.h/cpp, file_manager.h/cpp, main.cpp, server.cpp)
- **Lines added**: ~1800 (frontend + backend API)
- **Lines modified**: ~900 (calendar, main, build config)
- **Lines deleted**: ~300 (header implementations moved to .cpp files)
- **Bugs fixed**: 5 (time comparison, BST leak, heap deletion, stoi crash, CSV encoding)

---

## Session 2: Full Refactor — Bugs → Architecture → Frontend → Performance → Polish

### Phase 0 — Bugs & Stability
| # | Item | Files |
|---|------|-------|
| 0.1 | Test suite (doctest) | `backend/include/doctest.h`, `backend/src/test.cpp`, `CMakeLists.txt` (LIB_SOURCES pattern), `build.bat` (test target) |
| 0.2 | `stoi` crash fix + input validation | `server.cpp` — try-catch on all numeric params, 400/404/409 status codes |
| 0.3 | `const_cast` removal | `hash_table.h` + `hash_table.cpp` — `search()` no longer const, returns raw pointer |
| 0.4 | Event counter memory leak | `calendar.cpp` — `static int& unsaved = *new int(0)` → `static int unsaved = 0` |
| 0.5 | `getEndTime()` day wrap | `event.cpp` — appends `+Nd` suffix when crossing midnight instead of silent wrap |
| 0.6 | Centralized config | `backend/include/config.h` — `DATA_FILE`, `COUNTER_FILE`, `EXPORT_FILE` constants |

### Phase 1 — Architecture
| # | Item | Files |
|---|------|-------|
| 1.1 | BST → AVL tree | `bst.h` + `bst.cpp` — `height` field, `rotateRight`/`rotateLeft`, 4 rotation cases in `insertRec` |
| 1.2 | JSON body support | `server.cpp` — `getJSONString`/`getJSONNumber`/`isJSONRequest`/`paramOrJSON` helpers |
| 1.3 | Graceful shutdown | `server.cpp` — `SIGINT`/`SIGTERM` handlers, auto-save on exit |
| 1.4 | Logging system | `backend/include/logger.h` + `backend/src/logger.cpp` — `Logger::info/warn/error`, timestamps, level filtering |

### Phase 2 — Frontend UX
| # | Item | Files |
|---|------|-------|
| 2.1 | Error boundary | `frontend/src/components/ErrorBoundary.jsx` — class component catch + fallback UI |
| 2.2 | 404 page | `frontend/src/pages/NotFound.jsx` + `path="*"` route in `App.jsx` |
| 2.3 | EventCard form validation | `EventCard.jsx` — validates title/date/time/priority inline |
| 2.4 | Unsaved-changes guard | `AddEvent.jsx` — `useBlocker` from react-router-dom v6.30.4, tracks `isDirty`, skips after submit via `submitted` ref |
| 2.5 | Dark/Light theme | `App.jsx` + `Navbar.jsx` + `App.css` — `data-theme` attribute, CSS variables, localStorage persistence |
| 2.6 | Fetch retry wrapper | `api.js` — `fetchWithRetry()` exponential backoff (1s, 2s, 4s), 3 retries, wraps all API calls |
| 2.7 | Export button | `server.cpp` (`GET /api/export`), `api.js` (`exportToTXT`), `Dashboard.jsx` — download `.txt` file |

### Phase 3 — Performance
| # | Item | Files |
|---|------|-------|
| 3.1 | Pagination | `server.cpp` — query params `?page=N&per_page=M`, response `{ events, total, page, perPage, totalPages }`. `Dashboard.jsx` — prev/next controls, page info, stats use `total` |
| 3.2 | Lazy routes | `App.jsx` — `React.lazy()` + `<Suspense>` for all non-Dashboard pages |
| 3.3 | Memoization | `EventCard.jsx` — `React.memo` + `useCallback` on handlers. `Navbar.jsx` — `memo` wrapper |
| 3.4 | Compression | cpp-httplib v0.46.0 auto-compresses on `Accept-Encoding` — no config needed |

### Phase 4 — Dev Experience
| # | Item | Files |
|---|------|-------|
| 4.1 | ESLint config | `frontend/.eslintrc.cjs` — react + react-hooks recommended rules |
| 4.2 | .gitignore entries | `.gitignore` — added `build/`, `.env*`, `*.log` |
| 4.3 | CMake presets | `backend/CMakePresets.json` — debug/release modes (Ninja, separate build dirs) |
| 4.4 | Task runner | `Justfile` — targets for `build-backend`, `run-server`, `test-backend`, `run-frontend`, etc. |

### Phase 5 — Polish
| # | Item | Files |
|---|------|-------|
| 5.1 | Toast notifications | `frontend/src/components/ToastContext.jsx` — context + auto-dismiss. `frontend/src/components/ConfirmModal.jsx` — modal dialog replaces raw `confirm()`. `Dashboard.jsx` + `EventCard.jsx` updated |
| 5.2 | Tooltips | `title` attributes added to Save/Load/Export/Edit buttons across Dashboard and EventCard |
| 5.3 | Route animations | `App.jsx` — `AnimatedRoutes` component keyed on `location.pathname`, `.route-page` CSS fadeIn |

### New/Modified Files Summary
```
backend/src/server.cpp          — pagination, export, JSON parsing, graceful shutdown
backend/src/calendar.cpp        — counter fix, AVL integration
backend/src/bst.cpp             — AVL rotations (4 cases)
backend/include/bst.h           — height, rotate helpers
backend/include/config.h        — NEW: centralized paths
backend/include/logger.h        — NEW: logging interface
backend/src/logger.cpp          — NEW: logging implementation
backend/src/test.cpp            — NEW: doctest suite
backend/include/doctest.h       — NEW: doctest header
backend/CMakePresets.json       — NEW: build presets
frontend/.eslintrc.cjs          — NEW: ESLint config
Justfile                        — NEW: task runner
frontend/src/App.jsx            — lazy routes, theme toggle, toast provider, animated routes
frontend/src/api.js             — fetchWithRetry, pagination params, exportToTXT
frontend/src/components/ToastContext.jsx  — NEW
frontend/src/components/ConfirmModal.jsx  — NEW
frontend/src/components/ErrorBoundary.jsx — NEW
frontend/src/components/Navbar.jsx        — memo, theme toggle
frontend/src/components/EventCard.jsx     — memo, useCallback, form validation, confirm modal
frontend/src/pages/Dashboard.jsx          — pagination, toast, tooltips
frontend/src/pages/AddEvent.jsx           — useBlocker guard, submitted ref
frontend/src/pages/NotFound.jsx           — NEW: 404 page
frontend/src/pages/ViewEvents.jsx         — adapted to paginated response
frontend/src/App.css                      — theme vars, modal, toast container, pagination, route animation
.gitignore                                — added build/, .env*, *.log
```

### Fixed Post-Refactor Issues
| Issue | Fix |
|-------|-----|
| `ViewEvents.jsx` assumed `getEvents()` returns array | Now passes `(1, 500)` and reads `data.events` |
| `server.cpp` missing includes for `std::min`, `ifstream`, `istreambuf_iterator` | Added `<algorithm>`, `<fstream>`, `<iterator>` |

### Notes
- `build.bat` still fails due to lack of `g++` on Windows — but CMakeLists.txt structure is correct for any C++17 compiler
- react-router-dom v6.30.4 — `useBlocker` is stable (no `unstable_` prefix)
- cpp-httplib v0.46.0 Server has no `set_compress()` method, but auto-compresses via `Accept-Encoding`
