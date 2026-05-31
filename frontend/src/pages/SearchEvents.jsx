import { useState } from 'react'
import { searchEvents } from '../api'
import EventCard from '../components/EventCard'

export default function SearchEvents() {
  const [query, setQuery] = useState('')
  const [results, setResults] = useState([])
  const [searched, setSearched] = useState(false)
  const [loading, setLoading] = useState(false)

  const handleSearch = async (e) => {
    e.preventDefault()
    if (!query.trim()) return
    setLoading(true)
    setSearched(true)
    try {
      const data = await searchEvents(query)
      setResults(Array.isArray(data) ? data : [])
    } catch { setResults([]) }
    setLoading(false)
  }

  return (
    <div className="page fade-in">
      <div className="page-header">
        <h1>Search Events</h1>
      </div>

      <form className="search-bar" onSubmit={handleSearch}>
        <input
          value={query}
          onChange={e => setQuery(e.target.value)}
          placeholder="Search by title or ID..."
          className="search-input"
          autoFocus
        />
        <button type="submit" className="btn btn-primary" disabled={loading}>
          {loading ? 'Searching...' : 'Search'}
        </button>
      </form>

      {loading && <div className="loading-spinner" />}

      {searched && !loading && (
        <>
          <p className="result-count">{results.length} result{results.length !== 1 ? 's' : ''} found</p>
          {results.length === 0 ? (
            <div className="empty-state">
              <span className="empty-icon">🔍</span>
              <p>No matches for "{query}"</p>
            </div>
          ) : (
            <div className="events-grid">
              {results.map(e => (
                <EventCard key={e.id} event={e} />
              ))}
            </div>
          )}
        </>
      )}
    </div>
  )
}
