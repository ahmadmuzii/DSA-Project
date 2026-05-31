import { Routes, Route } from 'react-router-dom'
import Navbar from './components/Navbar'
import Dashboard from './pages/Dashboard'
import AddEvent from './pages/AddEvent'
import ViewEvents from './pages/ViewEvents'
import SearchEvents from './pages/SearchEvents'
import CheckConflicts from './pages/CheckConflicts'
import Structures from './pages/Structures'

export default function App() {
  return (
    <div className="app">
      <Navbar />
      <main className="main-content">
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="/add" element={<AddEvent />} />
          <Route path="/view" element={<ViewEvents />} />
          <Route path="/search" element={<SearchEvents />} />
          <Route path="/conflicts" element={<CheckConflicts />} />
          <Route path="/structures" element={<Structures />} />
        </Routes>
      </main>
    </div>
  )
}
