import { useState } from 'react'

export default function ConfirmModal({ message, onConfirm, onCancel }) {
  const [closing, setClosing] = useState(false)

  const handle = (fn) => () => {
    setClosing(true)
    setTimeout(fn, 200)
  }

  return (
    <div className={`modal-overlay ${closing ? 'closing' : ''}`} onClick={handle(onCancel)}>
      <div className="modal-box" onClick={e => e.stopPropagation()}>
        <p className="modal-message">{message}</p>
        <div className="modal-actions">
          <button className="btn btn-outline" onClick={handle(onCancel)}>Cancel</button>
          <button className="btn btn-delete" onClick={handle(onConfirm)}>Confirm</button>
        </div>
      </div>
    </div>
  )
}
