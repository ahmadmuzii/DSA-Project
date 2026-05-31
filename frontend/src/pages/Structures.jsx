import { useState, useEffect } from 'react'
import { getStructures } from '../api'

export default function Structures() {
  const [data, setData] = useState(null)
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    (async () => {
      try {
        const res = await getStructures()
        setData(res)
      } catch { setData({ info: 'Failed to fetch' }) }
      setLoading(false)
    })()
  }, [])

  return (
    <div className="page fade-in">
      <div className="page-header">
        <h1>Data Structures</h1>
      </div>

      {loading ? (
        <div className="loading-spinner" />
      ) : (
        <div className="structures-card">
          <div className="structure-icon">
            <span>🏗️</span>
          </div>
          <pre className="structure-output">{data?.info || 'No data'}</pre>
        </div>
      )}
    </div>
  )
}
