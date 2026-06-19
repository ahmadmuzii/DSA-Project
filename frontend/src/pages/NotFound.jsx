import { Link } from 'react-router-dom'

export default function NotFound() {
  return (
    <div className="page fade-in" style={{ textAlign: 'center', paddingTop: '80px' }}>
      <div className="page-header" style={{ justifyContent: 'center' }}>
        <h1>404 — Page Not Found</h1>
      </div>
      <div className="empty-state">
        <span className="empty-icon" style={{ fontSize: '4rem' }}>🔍</span>
        <p>The page you're looking for doesn't exist.</p>
        <Link to="/" className="btn btn-primary" style={{ display: 'inline-block', marginTop: '20px', textDecoration: 'none' }}>
          Go to Dashboard
        </Link>
      </div>
    </div>
  )
}
