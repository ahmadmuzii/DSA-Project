const BASE = '/api'

async function fetchWithRetry(url, options = {}, retries = 3) {
  for (let i = 0; i < retries; i++) {
    try {
      const res = await fetch(url, options)
      return res
    } catch (err) {
      if (i === retries - 1) throw err
      await new Promise(r => setTimeout(r, (1 << i) * 1000))
    }
  }
}

export async function getEvents(page = 1, perPage = 12) {
  const res = await fetchWithRetry(`${BASE}/events?page=${page}&per_page=${perPage}`)
  return res.json()
}

export async function getEventsByDate(date) {
  const res = await fetchWithRetry(`${BASE}/events/date/${encodeURIComponent(date)}`)
  return res.json()
}

export async function searchEvents(query) {
  const res = await fetchWithRetry(`${BASE}/events/search?q=${encodeURIComponent(query)}`)
  return res.json()
}

export async function checkConflicts(date) {
  const res = await fetchWithRetry(`${BASE}/events/conflicts/${encodeURIComponent(date)}`)
  return res.json()
}

export async function addEvent(event) {
  const params = new URLSearchParams(event)
  const res = await fetchWithRetry(`${BASE}/events`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: params
  })
  return res.json()
}

export async function getEventById(id) {
  const res = await fetchWithRetry(`${BASE}/events/${id}`)
  return res.json()
}

export async function updateEvent(id, event) {
  const params = new URLSearchParams(event)
  const res = await fetchWithRetry(`${BASE}/events/${id}`, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: params
  })
  return res.json()
}

export async function deleteEvent(id) {
  const res = await fetchWithRetry(`${BASE}/events/${id}`, { method: 'DELETE' })
  return res.json()
}

export async function getStructures() {
  const res = await fetchWithRetry(`${BASE}/structures`)
  return res.json()
}

export async function saveToFile() {
  const res = await fetchWithRetry(`${BASE}/save`, { method: 'POST' })
  return res.json()
}

export async function loadFromFile() {
  const res = await fetchWithRetry(`${BASE}/load`, { method: 'POST' })
  return res.json()
}

export async function exportToTXT() {
  const res = await fetchWithRetry(`${BASE}/export`)
  if (!res.ok) throw new Error('Export failed')
  return res.text()
}
