project_root := "."

# ─── Backend ─────────────────────────────────────────

# Build backend (debug)
build-backend:
    cd backend && cmake --preset debug && cmake --build --preset debug

# Build backend (release)
build-backend-release:
    cd backend && cmake --preset release && cmake --build --preset release

# Run backend server
run-server: build-backend
    ./backend/build/debug/calendar_server.exe

# Run backend CLI
run-cli: build-backend
    ./backend/build/debug/calendar_cli.exe

# Run backend tests
test-backend: build-backend
    ./backend/build/debug/test_runner.exe

# ─── Frontend ────────────────────────────────────────

# Install frontend dependencies
install-frontend:
    cd frontend && npm install

# Run frontend dev server
run-frontend:
    cd frontend && npm run dev

# Build frontend
build-frontend:
    cd frontend && npm run build

# Preview frontend build
preview-frontend:
    cd frontend && npm run preview

# ─── All ─────────────────────────────────────────────

# Run both servers (backend + frontend) concurrently
run-all:
    @echo "Start backend and frontend in separate terminals:"
    @echo "  just run-server"
    @echo "  just run-frontend"

# Default task
default: run-all
