import { useState, useEffect, lazy, Suspense } from 'react'
import { Routes, Route, useLocation } from 'react-router-dom'
import Navbar from './components/Navbar'
import ErrorBoundary from './components/ErrorBoundary'
import { ToastProvider } from './components/ToastContext'
import Dashboard from './pages/Dashboard'

const AddEvent = lazy(() => import('./pages/AddEvent'))
const ViewEvents = lazy(() => import('./pages/ViewEvents'))
const SearchEvents = lazy(() => import('./pages/SearchEvents'))
const CheckConflicts = lazy(() => import('./pages/CheckConflicts'))
const Structures = lazy(() => import('./pages/Structures'))
const NotFound = lazy(() => import('./pages/NotFound'))

function LoadingFallback() {
  return <div className="loading-spinner" />
}

function AnimatedRoutes() {
  const location = useLocation()
  return (
    <div key={location.pathname} className="route-page">
      <Suspense fallback={<LoadingFallback />}>
        <Routes location={location}>
          <Route path="/" element={<Dashboard />} />
          <Route path="/add" element={<AddEvent />} />
          <Route path="/view" element={<ViewEvents />} />
          <Route path="/search" element={<SearchEvents />} />
          <Route path="/conflicts" element={<CheckConflicts />} />
          <Route path="/structures" element={<Structures />} />
          <Route path="*" element={<NotFound />} />
        </Routes>
      </Suspense>
    </div>
  )
}

export default function App() {
  const [theme, setTheme] = useState(() => localStorage.getItem('theme') || 'dark')

  useEffect(() => {
    document.documentElement.setAttribute('data-theme', theme)
    localStorage.setItem('theme', theme)
  }, [theme])

  const toggleTheme = () => setTheme(t => t === 'dark' ? 'light' : 'dark')

  return (
    <ErrorBoundary>
      <ToastProvider>
      <div className="app">
        <Navbar theme={theme} onToggleTheme={toggleTheme} />
        <main className="main-content">
          <AnimatedRoutes />
        </main>
      </div>
      </ToastProvider>
    </ErrorBoundary>
  )
}
