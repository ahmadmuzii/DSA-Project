import { useState } from 'react'
import { deleteEvent, updateEvent } from '../api'

const priorityColors = {
  1: '#ff4757',
  2: '#ff6348',
  3: '#ffa502',
  4: '#2ed573',
  5: '#1e90ff'
}

const priorityLabels = {
  1: 'Highest',
  2: 'High',
  3: 'Medium',
  4: 'Low',
  5: 'Lowest'
}

export default function EventCard({ event, onUpdate }) {
  const [deleting, setDeleting] = useState(false)
  const [editing, setEditing] = useState(false)
  const [form, setForm] = useState({ ...event })

  const handleDelete = async () => {
    if (!confirm(`Delete "${event.title}"?`)) return
    setDeleting(true)
    await deleteEvent(event.id)
    onUpdate?.()
  }

  const handleUpdate = async () => {
    await updateEvent(event.id, form)
    setEditing(false)
    onUpdate?.()
  }

  if (editing) {
    return (
      <div className="event-card editing">
        <input value={form.title} onChange={e => setForm({ ...form, title: e.target.value })} placeholder="Title" />
        <input value={form.date} onChange={e => setForm({ ...form, date: e.target.value })} placeholder="DD:MM:YYYY" />
        <input value={form.startTime} onChange={e => setForm({ ...form, startTime: e.target.value })} placeholder="HH:MM" />
        <input type="number" value={form.durationMins} onChange={e => setForm({ ...form, durationMins: +e.target.value })} placeholder="Duration (min)" />
        <select value={form.priority} onChange={e => setForm({ ...form, priority: +e.target.value })}>
          {[1,2,3,4,5].map(p => <option key={p} value={p}>{p} - {priorityLabels[p]}</option>)}
        </select>
        <input value={form.description} onChange={e => setForm({ ...form, description: e.target.value })} placeholder="Description" />
        <div className="card-actions">
          <button className="btn btn-save" onClick={handleUpdate}>Save</button>
          <button className="btn btn-cancel" onClick={() => setEditing(false)}>Cancel</button>
        </div>
      </div>
    )
  }

  return (
    <div className="event-card" style={{ borderLeft: `4px solid ${priorityColors[event.priority] || '#ffa502'}` }}>
      <div className="event-card-header">
        <h3 className="event-title">{event.title}</h3>
        <span className="event-priority" style={{ background: priorityColors[event.priority] || '#ffa502' }}>
          {priorityLabels[event.priority]}
        </span>
      </div>
      <div className="event-card-body">
        <div className="event-detail">
          <span className="detail-label">Date</span>
          <span className="detail-value">{event.date}</span>
        </div>
        <div className="event-detail">
          <span className="detail-label">Time</span>
          <span className="detail-value">{event.startTime} - {event.endTime}</span>
        </div>
        <div className="event-detail">
          <span className="detail-label">Duration</span>
          <span className="detail-value">{event.durationMins} min</span>
        </div>
        <div className="event-detail">
          <span className="detail-label">ID</span>
          <span className="detail-value id">{event.id}</span>
        </div>
        {event.description && (
          <div className="event-detail">
            <span className="detail-label">Description</span>
            <span className="detail-value">{event.description}</span>
          </div>
        )}
      </div>
      <div className="card-actions">
        <button className="btn btn-edit" onClick={() => setEditing(true)} disabled={deleting}>Edit</button>
        <button className="btn btn-delete" onClick={handleDelete} disabled={deleting}>
          {deleting ? 'Deleting...' : 'Delete'}
        </button>
      </div>
    </div>
  )
}
