import { useState, useEffect, useCallback } from 'react'
import { getEvents, saveToFile, loadFromFile, exportToTXT } from '../api'
import EventCard from '../components/EventCard'
import { useToast } from '../components/ToastContext'

export default function Dashboard() {
  const showToast = useToast()
  const [events, setEvents] = useState([])
  const [loading, setLoading] = useState(true)
  const [page, setPage] = useState(1)
  const [totalPages, setTotalPages] = useState(1)
  const [total, setTotal] = useState(0)
  const perPage = 12

  const fetchEvents = useCallback(async (p = page) => {
    setLoading(true)
    try {
      const data = await getEvents(p, perPage)
      setEvents(Array.isArray(data.events) ? data.events : [])
      setTotal(data.total || 0)
      setTotalPages(data.totalPages || 1)
      setPage(data.page || 1)
    } catch (e) {
      setEvents([])
    }
    setLoading(false)
  }, [page])

  useEffect(() => { fetchEvents() }, [fetchEvents])

  const handleSave = async () => {
    const res = await saveToFile()
    showToast(res.success ? 'Data saved!' : 'Save failed!', res.success ? 'success' : 'error')
  }

  const handleLoad = async () => {
    if (!confirm('Replace all current events with saved data?')) return
    const res = await loadFromFile()
    showToast(`Loaded ${res.count || 0} events!`, 'success')
    fetchEvents()
  }

  const handleExport = async () => {
    try {
      const content = await exportToTXT()
      const blob = new Blob([content], { type: 'text/plain' })
      const url = URL.createObjectURL(blob)
      const a = document.createElement('a')
      a.href = url
      a.download = 'calendar_export.txt'
      a.click()
      URL.revokeObjectURL(url)
      showToast('Exported successfully!', 'success')
    } catch {
      showToast('Export failed!', 'error')
    }
  }

  return (
    <div className="page fade-in">
      <div className="page-header">
        <h1>Dashboard</h1>
        <div className="header-actions">
          <button className="btn btn-outline" onClick={handleSave} title="Save events to file">Save</button>
          <button className="btn btn-outline" onClick={handleLoad} title="Load events from file">Load</button>
          <button className="btn btn-outline" onClick={handleExport} title="Export events as TXT">Export</button>
        </div>
      </div>

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
        <>
          <div className="events-grid">
            {events.map(e => (
              <EventCard key={e.id} event={e} onUpdate={() => fetchEvents(page)} />
            ))}
          </div>

          <div className="pagination">
            <button className="btn btn-outline" disabled={page <= 1} onClick={() => fetchEvents(page - 1)}>
              ← Prev
            </button>
            <span className="pagination-info">Page {page} of {totalPages} ({total} events)</span>
            <button className="btn btn-outline" disabled={page >= totalPages} onClick={() => fetchEvents(page + 1)}>
              Next →
            </button>
          </div>
        </>
      )}
    </div>
  )
}
