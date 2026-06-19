import { useState, useCallback, memo } from 'react'
import { deleteEvent, updateEvent } from '../api'
import { useToast } from './ToastContext'
import ConfirmModal from './ConfirmModal'

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

function EventCard({ event, onUpdate }) {
  const showToast = useToast()
  const [confirmDelete, setConfirmDelete] = useState(false)
  const [deleting, setDeleting] = useState(false)
  const [editing, setEditing] = useState(false)
  const [form, setForm] = useState({ ...event })
  const [editError, setEditError] = useState('')

  const handleDelete = useCallback(async () => {
    setDeleting(true)
    await deleteEvent(event.id)
    showToast(`Deleted "${event.title}"`, 'success')
    onUpdate?.()
  }, [event.id, event.title, onUpdate, showToast])

  const validateEdit = useCallback(() => {
    if (!form.title.trim()) return 'Title is required'
    if (!/^\d{2}:\d{2}:\d{4}$/.test(form.date)) return 'Date must be DD:MM:YYYY'
    if (!/^\d{2}:\d{2}$/.test(form.startTime)) return 'Start time must be HH:MM'
    if (form.priority < 1 || form.priority > 5) return 'Priority must be 1-5'
    return ''
  }, [form])

  const handleUpdate = useCallback(async () => {
    const err = validateEdit()
    if (err) { setEditError(err); return }
    setEditError('')
    await updateEvent(event.id, form)
    setEditing(false)
    showToast('Event updated!', 'success')
    onUpdate?.()
  }, [event.id, form, onUpdate, validateEdit, showToast])

  if (editing) {
    return (
      <div className="event-card editing">
        {editError && <div className="form-error" style={{ marginBottom: '8px' }}>{editError}</div>}
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
          <button className="btn btn-cancel" onClick={() => { setEditing(false); setEditError('') }}>Cancel</button>
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
        <button className="btn btn-edit" onClick={() => setEditing(true)} disabled={deleting} title="Edit this event">Edit</button>
        <button className="btn btn-delete" onClick={() => setConfirmDelete(true)} disabled={deleting}>
          {deleting ? 'Deleting...' : 'Delete'}
        </button>
      </div>
      {confirmDelete && (
        <ConfirmModal
          message={`Delete "${event.title}"?`}
          onConfirm={() => { setConfirmDelete(false); handleDelete() }}
          onCancel={() => setConfirmDelete(false)}
        />
      )}
    </div>
  )
}

export default memo(EventCard)
