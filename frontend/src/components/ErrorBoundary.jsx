import { Component } from 'react'

export default class ErrorBoundary extends Component {
  constructor(props) {
    super(props)
    this.state = { hasError: false, error: null }
  }

  static getDerivedStateFromError(error) {
    return { hasError: true, error }
  }

  render() {
    if (this.state.hasError) {
      return (
        <div className="page fade-in" style={{ textAlign: 'center', paddingTop: '80px' }}>
          <div className="page-header" style={{ justifyContent: 'center' }}>
            <h1>Something went wrong</h1>
          </div>
          <div className="empty-state">
            <span className="empty-icon" style={{ fontSize: '4rem' }}>⚠️</span>
            <p>{this.state.error?.message || 'An unexpected error occurred'}</p>
            <button
              className="btn btn-primary"
              style={{ marginTop: '20px' }}
              onClick={() => { this.setState({ hasError: false }); window.location.href = '/' }}
            >
              Go to Dashboard
            </button>
          </div>
        </div>
      )
    }
    return this.props.children
  }
}
