const BASE = '/api'

export async function getEvents() {
  const res = await fetch(`${BASE}/events`)
  return res.json()
}

export async function getEventsByDate(date) {
  const res = await fetch(`${BASE}/events/date/${encodeURIComponent(date)}`)
  return res.json()
}

export async function searchEvents(query) {
  const res = await fetch(`${BASE}/events/search?q=${encodeURIComponent(query)}`)
  return res.json()
}

export async function checkConflicts(date) {
  const res = await fetch(`${BASE}/events/conflicts/${encodeURIComponent(date)}`)
  return res.json()
}

export async function addEvent(event) {
  const params = new URLSearchParams(event)
  const res = await fetch(`${BASE}/events`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: params
  })
  return res.json()
}

export async function getEventById(id) {
  const res = await fetch(`${BASE}/events/${id}`)
  return res.json()
}

export async function updateEvent(id, event) {
  const params = new URLSearchParams(event)
  const res = await fetch(`${BASE}/events/${id}`, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: params
  })
  return res.json()
}

export async function deleteEvent(id) {
  const res = await fetch(`${BASE}/events/${id}`, { method: 'DELETE' })
  return res.json()
}

export async function getStructures() {
  const res = await fetch(`${BASE}/structures`)
  return res.json()
}

export async function saveToFile() {
  const res = await fetch(`${BASE}/save`, { method: 'POST' })
  return res.json()
}

export async function loadFromFile() {
  const res = await fetch(`${BASE}/load`, { method: 'POST' })
  return res.json()
}
