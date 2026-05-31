import { useState } from 'react'
import { addEvent } from '../api'
import { useNavigate } from 'react-router-dom'

export default function AddEvent() {
  const navigate = useNavigate()
  const [form, setForm] = useState({
    title: '',
    date: '',
    startTime: '',
    duration: '60',
    priority: '3',
    description: ''
  })
  const [error, setError] = useState('')
  const [saving, setSaving] = useState(false)

  const validate = () => {
    if (!form.title.trim()) return 'Title is required'
    if (!/^\d{2}:\d{2}:\d{4}$/.test(form.date)) return 'Date must be DD:MM:YYYY'
    if (!/^\d{2}:\d{2}$/.test(form.startTime)) return 'Time must be HH:MM'
    return ''
  }

  const handleSubmit = async (e) => {
    e.preventDefault()
    const err = validate()
    if (err) { setError(err); return }
    setSaving(true)
    setError('')
    try {
      const res = await addEvent(form)
      if (res.success) {
        navigate('/')
      } else {
        setError(res.error || 'Failed to add event')
      }
    } catch {
      setError('Server connection failed')
    }
    setSaving(false)
  }

  const handleChange = (field) => (e) => setForm({ ...form, [field]: e.target.value })

  return (
    <div className="page fade-in">
      <div className="page-header">
        <h1>Add New Event</h1>
      </div>

      <form className="event-form" onSubmit={handleSubmit}>
        {error && <div className="form-error">{error}</div>}

        <div className="form-group">
          <label>Title</label>
          <input value={form.title} onChange={handleChange('title')} placeholder="Event title" required />
        </div>

        <div className="form-row">
          <div className="form-group">
            <label>Date (DD:MM:YYYY)</label>
            <input value={form.date} onChange={handleChange('date')} placeholder="15:03:2026" />
          </div>
          <div className="form-group">
            <label>Start Time (HH:MM)</label>
            <input value={form.startTime} onChange={handleChange('startTime')} placeholder="14:30" />
          </div>
        </div>

        <div className="form-row">
          <div className="form-group">
            <label>Duration (minutes)</label>
            <input type="number" value={form.duration} onChange={handleChange('duration')} min="1" />
          </div>
          <div className="form-group">
            <label>Priority</label>
            <select value={form.priority} onChange={handleChange('priority')}>
              <option value="1">1 - Highest</option>
              <option value="2">2 - High</option>
              <option value="3">3 - Medium</option>
              <option value="4">4 - Low</option>
              <option value="5">5 - Lowest</option>
            </select>
          </div>
        </div>

        <div className="form-group">
          <label>Description (optional)</label>
          <textarea value={form.description} onChange={handleChange('description')} rows={3} placeholder="Event description..." />
        </div>

        <button type="submit" className="btn btn-primary" disabled={saving}>
          {saving ? 'Adding...' : 'Add Event'}
        </button>
      </form>
    </div>
  )
}
