import { useEffect, useState, type ReactNode } from 'react'
import { Navigate, Route, Routes } from 'react-router-dom'

import Landing from './pages/Landing'
import Login from './pages/Login'
import Signup from './pages/Signup'
import Dashboard from './pages/Dashboard'
import Translator from './pages/Translator'
import History from './pages/History'
import Saved from './pages/Saved'
import Statistics from './pages/Statistics'
import Profile from './pages/Profile'
import Settings from './pages/Settings'
import { API_BASE_URL } from './utils/api'

/* =========================================================
   AUTHENTICATION CHECK
   ========================================================= */

function getStoredToken(): string {
  return (
    localStorage.getItem('morselab_token') ||
    sessionStorage.getItem('morselab_token') ||
    ''
  )
}

function clearStoredSession(): void {
  localStorage.removeItem('morselab_token')
  localStorage.removeItem('morselab_user')
  sessionStorage.removeItem('morselab_token')
  sessionStorage.removeItem('morselab_user')
}

/* =========================================================
   PROTECTED ROUTE
   ========================================================= */

function ProtectedRoute({
  children,
}: {
  children: ReactNode
}) {
  const [status, setStatus] = useState<'checking' | 'authenticated' | 'unauthenticated'>(
    () => (getStoredToken() ? 'checking' : 'unauthenticated'),
  )

  useEffect(() => {
    let active = true
    const token = getStoredToken()

    if (!token) {
      setStatus('unauthenticated')
      return
    }

    fetch(`${API_BASE_URL}/api/auth/validate`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ token }),
    })
      .then(async (response) => {
        const data = await response.json()
        if (!response.ok || !data.success || data.authenticated !== true) {
          clearStoredSession()
          if (active) {
            setStatus('unauthenticated')
          }
          return
        }

        if (active) {
          setStatus('authenticated')
        }
      })
      .catch(() => {
        clearStoredSession()
        if (active) {
          setStatus('unauthenticated')
        }
      })

    return () => {
      active = false
    }
  }, [])

  if (status === 'checking') {
    return (
      <div
        className="flex min-h-screen items-center justify-center bg-[#050816] text-sm text-slate-400"
        role="status"
      >
        Verifying your session...
      </div>
    )
  }

  if (status !== 'authenticated') {
    return <Navigate to="/login" replace />
  }

  return children
}

/* =========================================================
   APPLICATION ROUTES
   ========================================================= */

function App() {
  return (
    <Routes>
      {/* Public routes */}
      <Route path="/" element={<Landing />} />
      <Route path="/login" element={<Login />} />
      <Route path="/signup" element={<Signup />} />

      {/* Protected routes */}
      <Route
        path="/dashboard"
        element={
          <ProtectedRoute>
            <Dashboard />
          </ProtectedRoute>
        }
      />

      <Route
        path="/translator"
        element={
          <ProtectedRoute>
            <Translator />
          </ProtectedRoute>
        }
      />

      <Route
        path="/history"
        element={
          <ProtectedRoute>
            <History />
          </ProtectedRoute>
        }
      />

      <Route
        path="/saved"
        element={
          <ProtectedRoute>
            <Saved />
          </ProtectedRoute>
        }
      />

      <Route
        path="/statistics"
        element={
          <ProtectedRoute>
            <Statistics />
          </ProtectedRoute>
        }
      />

      <Route
        path="/profile"
        element={
          <ProtectedRoute>
            <Profile />
          </ProtectedRoute>
        }
      />

      <Route
        path="/settings"
        element={
          <ProtectedRoute>
            <Settings />
          </ProtectedRoute>
        }
      />

      {/* Unknown route */}
      <Route
        path="*"
        element={<Navigate to="/" replace />}
      />
    </Routes>
  )
}

export default App
