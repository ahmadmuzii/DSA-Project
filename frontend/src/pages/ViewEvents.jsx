import { useState, useEffect, useCallback } from 'react'
import { getEventsByDate, getEvents } from '../api'
import EventCard from '../components/EventCard'

export default function ViewEvents() {
  const [date, setDate] = useState('')
  const [events, setEvents] = useState([])
  const [allEvents, setAllEvents] = useState([])
  const [loading, setLoading] = useState(false)
  const [mode, setMode] = useState('all')

  const fetchAll = useCallback(async () => {
    const data = await getEvents()
    setAllEvents(Array.isArray(data) ? data : [])
  }, [])

  useEffect(() => { fetchAll() }, [fetchAll])

  const handleSearch = async (e) => {
    e?.preventDefault()
    if (!date) return
    setLoading(true)
    setMode('date')
    try {
      const data = await getEventsByDate(date)
      setEvents(Array.isArray(data) ? data : [])
    } catch { setEvents([]) }
    setLoading(false)
  }

  const eventsToShow = mode === 'all' ? allEvents : events
  const grouped = mode === 'all'
    ? Object.entries(
        allEvents.reduce((acc, e) => {
          (acc[e.date] = acc[e.date] || []).push(e)
          return acc
        }, {})
      ).sort(([a], [b]) => a.localeCompare(b))
    : [[date, events]]

  return (
    <div className="page fade-in">
      <div className="page-header">
        <h1>View Events</h1>
      </div>

      <form className="search-bar" onSubmit={handleSearch}>
        <input
          value={date}
          onChange={e => setDate(e.target.value)}
          placeholder="Filter by date (DD:MM:YYYY)"
          className="search-input"
        />
        <button type="submit" className="btn btn-primary">Filter</button>
        <button type="button" className="btn btn-outline" onClick={() => { setMode('all'); setDate('') }}>
          Show All
        </button>
      </form>

      {loading ? (
        <div className="loading-spinner" />
      ) : grouped.length === 0 ? (
        <div className="empty-state">
          <span className="empty-icon">📭</span>
          <p>No events found</p>
        </div>
      ) : (
        grouped.map(([d, evts]) => (
          <div key={d} className="date-group">
            <h2 className="date-heading">{d}</h2>
            <div className="events-grid">
              {evts.map(e => (
                <EventCard key={e.id} event={e} onUpdate={fetchAll} />
              ))}
            </div>
          </div>
        ))
      )}
    </div>
  )
}
