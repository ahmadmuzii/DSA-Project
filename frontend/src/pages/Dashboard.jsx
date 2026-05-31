import { useState, useEffect, useCallback } from 'react'
import { getEvents, saveToFile, loadFromFile } from '../api'
import EventCard from '../components/EventCard'

export default function Dashboard() {
  const [events, setEvents] = useState([])
  const [loading, setLoading] = useState(true)
  const [message, setMessage] = useState('')

  const fetchEvents = useCallback(async () => {
    setLoading(true)
    try {
      const data = await getEvents()
      setEvents(Array.isArray(data) ? data : [])
    } catch (e) {
      setEvents([])
    }
    setLoading(false)
  }, [])

  useEffect(() => { fetchEvents() }, [fetchEvents])

  const handleSave = async () => {
    const res = await saveToFile()
    setMessage(res.success ? 'Data saved successfully!' : 'Save failed!')
    setTimeout(() => setMessage(''), 3000)
  }

  const handleLoad = async () => {
    if (!confirm('Replace all current events with saved data?')) return
    const res = await loadFromFile()
    setMessage(`Loaded ${res.count || 0} events!`)
    fetchEvents()
    setTimeout(() => setMessage(''), 3000)
  }

  return (
    <div className="page fade-in">
      <div className="page-header">
        <h1>Dashboard</h1>
        <div className="header-actions">
          <button className="btn btn-outline" onClick={handleSave}>Save to File</button>
          <button className="btn btn-outline" onClick={handleLoad}>Load from File</button>
        </div>
      </div>

      {message && <div className="toast">{message}</div>}

      <div className="stats-row">
        <div className="stat-card">
          <span className="stat-number">{events.length}</span>
          <span className="stat-label">Total Events</span>
        </div>
        <div className="stat-card">
          <span className="stat-number">
            {events.filter(e => e.priority <= 2).length}
          </span>
          <span className="stat-label">High Priority</span>
        </div>
        <div className="stat-card">
          <span className="stat-number">
            {new Set(events.map(e => e.date)).size}
          </span>
          <span className="stat-label">Active Dates</span>
        </div>
      </div>

      <h2 className="section-title">Upcoming Events</h2>
      {loading ? (
        <div className="loading-spinner" />
      ) : events.length === 0 ? (
        <div className="empty-state">
          <span className="empty-icon">📅</span>
          <p>No events yet. Add one to get started!</p>
        </div>
      ) : (
        <div className="events-grid">
          {events.map(e => (
            <EventCard key={e.id} event={e} onUpdate={fetchEvents} />
          ))}
        </div>
      )}
    </div>
  )
}
