import { useState } from 'react'
import { checkConflicts, getEventsByDate } from '../api'

const conflictColors = ['#ff4757', '#ff6348', '#ffa502']

export default function CheckConflicts() {
  const [date, setDate] = useState('')
  const [data, setData] = useState(null)
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState('')

  const handleCheck = async (e) => {
    e.preventDefault()
    if (!/^\d{2}:\d{2}:\d{4}$/.test(date)) {
      setError('Date must be DD:MM:YYYY')
      return
    }
    setError('')
    setLoading(true)
    try {
      const res = await checkConflicts(date)
      const events = await getEventsByDate(date)
      setData({
        conflicts: res.conflicts || [],
        count: res.conflictCount || 0,
        totalEvents: Array.isArray(events) ? events.length : 0
      })
    } catch {
      setData({ conflicts: [], count: 0, totalEvents: 0 })
    }
    setLoading(false)
  }

  return (
    <div className="page fade-in">
      <div className="page-header">
        <h1>Check Time Conflicts</h1>
      </div>

      <form className="search-bar" onSubmit={handleCheck}>
        <input
          value={date}
          onChange={e => setDate(e.target.value)}
          placeholder="Date (DD:MM:YYYY)"
          className="search-input"
        />
        <button type="submit" className="btn btn-primary" disabled={loading}>
          {loading ? 'Checking...' : 'Check'}
        </button>
      </form>

      {error && <div className="form-error">{error}</div>}

      {data && (
        <div className="conflict-results">
          <div className="stats-row">
            <div className="stat-card">
              <span className="stat-number">{data.totalEvents}</span>
              <span className="stat-label">Events on {date}</span>
            </div>
            <div className={`stat-card ${data.count > 0 ? 'warning' : 'success'}`}>
              <span className="stat-number">{data.count}</span>
              <span className="stat-label">Conflicts</span>
            </div>
          </div>

          {data.conflicts.length === 0 ? (
            <div className="empty-state">
              <span className="empty-icon">✅</span>
              <p>No overlapping events found for {date}</p>
            </div>
          ) : (
            <div className="conflict-list">
              {data.conflicts.map((c, i) => (
                <div key={i} className="conflict-pair slide-in" style={{ animationDelay: `${i * 0.1}s` }}>
                  <div className="conflict-badge" style={{ background: conflictColors[i % conflictColors.length] }}>
                    Conflict #{i + 1}
                  </div>
                  <div className="conflict-events">
                    <div className="conflict-event">
                      <strong>{c.eventA.title}</strong>
                      <span>{c.eventA.startTime} - {c.eventA.endTime}</span>
                      <span className="id">{c.eventA.id}</span>
                    </div>
                    <div className="conflict-vs">⛔ VS</div>
                    <div className="conflict-event">
                      <strong>{c.eventB.title}</strong>
                      <span>{c.eventB.startTime} - {c.eventB.endTime}</span>
                      <span className="id">{c.eventB.id}</span>
                    </div>
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      )}
    </div>
  )
}
