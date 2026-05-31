import { NavLink } from 'react-router-dom'

export default function Navbar() {
  const links = [
    { to: '/', label: 'Dashboard' },
    { to: '/add', label: 'Add Event' },
    { to: '/view', label: 'View Events' },
    { to: '/search', label: 'Search' },
    { to: '/conflicts', label: 'Conflicts' },
    { to: '/structures', label: 'Structures' },
  ]

  return (
    <nav className="navbar">
      <div className="navbar-brand">
        <span className="navbar-icon">📅</span>
        <span className="navbar-title">Calendar DSA</span>
      </div>
      <div className="navbar-links">
        {links.map(link => (
          <NavLink
            key={link.to}
            to={link.to}
            className={({ isActive }) => `nav-link ${isActive ? 'active' : ''}`}
            end={link.to === '/'}
          >
            {link.label}
          </NavLink>
        ))}
      </div>
    </nav>
  )
}
