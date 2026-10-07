import {
  ArrowLeft,
  AtSign,
  CalendarDays,
  CheckCircle2,
  Clock3,
  LockKeyhole,
  Mail,
  Save,
  ShieldCheck,
  User,
  Activity,
} from 'lucide-react'
import { useEffect, useState } from 'react'
import { Link } from 'react-router-dom'
import { notifyActivityIfEnabled } from '../utils/notifications'
import { API_BASE_URL } from '../utils/api'

type ProfileData = {
  id: number
  name: string
  email: string
  username: string
  created_at: string
  last_login: string
  translation_count: number
}

function Profile() {
  const [name, setName] = useState('')
  const [email, setEmail] = useState('')
  const [username, setUsername] = useState('')

  const [profile, setProfile] = useState<ProfileData | null>(null)

  const [loading, setLoading] = useState(true)
  const [saving, setSaving] = useState(false)

  const [message, setMessage] = useState('')
  const [error, setError] = useState('')

  const getToken = () => {
    return (
      localStorage.getItem('morselab_token') ||
      sessionStorage.getItem('morselab_token') ||
      ''
    )
  }

  const formatDate = (dateValue: string) => {
    if (!dateValue) {
      return 'Not available'
    }

    const date = new Date(dateValue.replace(' ', 'T'))

    if (Number.isNaN(date.getTime())) {
      return dateValue
    }

    return date.toLocaleString()
  }

  useEffect(() => {
    const loadProfile = async () => {
      const token = getToken()

      if (!token) {
        setError('You are not logged in.')
        setLoading(false)
        return
      }

      try {
        const response = await fetch(`${API_BASE_URL}/api/profile`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            token,
          }),
        })

        const data = await response.json()

        if (!response.ok || !data.success) {
          throw new Error(
            data.error || 'Unable to load your profile.'
          )
        }

        setProfile(data.profile)

        setName(data.profile.name || '')
        setEmail(data.profile.email || '')
        setUsername(data.profile.username || '')
      } catch (profileError) {
        setError(
          profileError instanceof Error
            ? profileError.message
            : 'Unable to load your profile.'
        )
      } finally {
        setLoading(false)
      }
    }

    loadProfile()
  }, [])

  const handleSave = async () => {
    const token = getToken()

    if (!token) {
      setError('You are not logged in.')
      return
    }

    if (!name.trim() || !email.trim() || !username.trim()) {
      setError('Name, email and username cannot be empty.')
      setMessage('')
      return
    }

    setSaving(true)
    setMessage('')
    setError('')

    try {
      const response = await fetch(
        `${API_BASE_URL}/api/profile/update`,
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            token,
            name: name.trim(),
            email: email.trim(),
            username: username.trim(),
          }),
        }
      )

      const data = await response.json()

      if (!response.ok || !data.success) {
        throw new Error(
          data.error || 'Unable to update your profile.'
        )
      }

      setMessage('Profile updated successfully.')
      void notifyActivityIfEnabled(
        token,
        'MorseLab Profile',
        'Your account profile was updated.',
      )

      setProfile((currentProfile) => {
        if (!currentProfile) {
          return currentProfile
        }

        return {
          ...currentProfile,
          name: name.trim(),
          email: email.trim(),
          username: username.trim(),
        }
      })

      const storedUser =
        localStorage.getItem('morselab_user') ||
        sessionStorage.getItem('morselab_user')

      if (storedUser) {
        try {
          const parsedUser = JSON.parse(storedUser)

          parsedUser.name = name.trim()

          const updatedUser = JSON.stringify(parsedUser)

          if (localStorage.getItem('morselab_user')) {
            localStorage.setItem(
              'morselab_user',
              updatedUser
            )
          } else {
            sessionStorage.setItem(
              'morselab_user',
              updatedUser
            )
          }
        } catch {
          // Ignore invalid stored user data.
        }
      }
    } catch (profileError) {
      setError(
        profileError instanceof Error
          ? profileError.message
          : 'Unable to update your profile.'
      )
    } finally {
      setSaving(false)
    }
  }

  return (
    <div className="min-h-screen bg-[#050816] text-white">
      <header className="border-b border-white/10 bg-[#070b1a]/90 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-6 py-5">
          <Link to="/dashboard" className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <User className="h-5 w-5 text-cyan-300" />
            </div>

            <div>
              <div className="font-bold tracking-widest">
                MORSELAB
              </div>

              <div className="text-xs text-slate-500">
                Profile
              </div>
            </div>
          </Link>

          <Link
            to="/dashboard"
            className="rounded-xl border border-white/10 px-4 py-2.5 text-sm font-medium text-slate-400 transition hover:bg-white/5 hover:text-white"
          >
            Dashboard
          </Link>
        </div>
      </header>

      <main className="mx-auto max-w-5xl px-6 py-10">
        <div className="mb-8">
          <Link
            to="/dashboard"
            className="mb-5 inline-flex items-center gap-2 text-sm text-slate-500 transition hover:text-white"
          >
            <ArrowLeft className="h-4 w-4" />
            Dashboard
          </Link>

          <h1 className="text-4xl font-bold tracking-tight">
            Profile Settings
          </h1>

          <p className="mt-3 max-w-2xl text-slate-400">
            Manage your account information and personal details.
          </p>
        </div>

        {loading ? (
          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-10 text-center">
            <div className="mx-auto h-8 w-8 animate-spin rounded-full border-2 border-cyan-400/20 border-t-cyan-400" />

            <p className="mt-4 text-sm text-slate-500">
              Loading your profile...
            </p>
          </div>
        ) : (
          <div className="grid gap-6 lg:grid-cols-[260px_1fr]">
            <aside className="h-fit rounded-2xl border border-white/10 bg-white/[0.03] p-5">
              <div className="flex flex-col items-center text-center">
                <div className="flex h-24 w-24 items-center justify-center rounded-3xl border border-cyan-400/20 bg-cyan-400/10">
                  <User className="h-10 w-10 text-cyan-300" />
                </div>

                <h2 className="mt-5 text-lg font-semibold">
                  {name || 'MorseLab User'}
                </h2>

                <p className="mt-1 text-sm text-slate-600">
                  @{username || 'MorseLabUser'}
                </p>
              </div>

              <div className="mt-6 space-y-2 border-t border-white/10 pt-5">
                <div className="flex items-center gap-3 rounded-xl bg-white/5 px-4 py-3 text-sm text-cyan-300">
                  <User className="h-4 w-4" />
                  Personal Information
                </div>

                <Link
                  to="/settings"
                  className="flex items-center gap-3 rounded-xl px-4 py-3 text-sm text-slate-500 transition hover:bg-white/5 hover:text-white"
                >
                  <LockKeyhole className="h-4 w-4" />
                  Security & Settings
                </Link>
              </div>
            </aside>

            <section className="rounded-2xl border border-white/10 bg-white/[0.03] p-6 md:p-8">
              <div className="mb-8">
                <h2 className="text-xl font-semibold">
                  Personal Information
                </h2>

                <p className="mt-2 text-sm text-slate-500">
                  Update the information associated with your MorseLab
                  account.
                </p>
              </div>

              {error && (
                <div className="mb-6 rounded-xl border border-red-400/20 bg-red-400/5 px-4 py-3 text-sm text-red-300">
                  {error}
                </div>
              )}

              {message && (
                <div className="mb-6 flex items-center gap-2 rounded-xl border border-emerald-400/20 bg-emerald-400/5 px-4 py-3 text-sm text-emerald-300">
                  <CheckCircle2 className="h-4 w-4" />
                  {message}
                </div>
              )}

              <div className="space-y-6">
                <div>
                  <label
                    htmlFor="profile-name"
                    className="mb-2 block text-sm font-medium text-slate-300"
                  >
                    Full Name
                  </label>

                  <div className="relative">
                    <User className="absolute left-4 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                    <input
                      id="profile-name"
                      type="text"
                      value={name}
                      onChange={(event) =>
                        setName(event.target.value)
                      }
                      placeholder="Enter your full name"
                      className="w-full rounded-xl border border-white/10 bg-black/20 py-3.5 pl-11 pr-4 text-sm text-white outline-none placeholder:text-slate-700 focus:border-cyan-400/40"
                    />
                  </div>
                </div>

                <div>
                  <label
                    htmlFor="profile-email"
                    className="mb-2 block text-sm font-medium text-slate-300"
                  >
                    Email Address
                  </label>

                  <div className="relative">
                    <Mail className="absolute left-4 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                    <input
                      id="profile-email"
                      type="email"
                      value={email}
                      onChange={(event) =>
                        setEmail(event.target.value)
                      }
                      placeholder="Enter your email address"
                      className="w-full rounded-xl border border-white/10 bg-black/20 py-3.5 pl-11 pr-4 text-sm text-white outline-none placeholder:text-slate-700 focus:border-cyan-400/40"
                    />
                  </div>
                </div>

                <div>
                  <label
                    htmlFor="profile-username"
                    className="mb-2 block text-sm font-medium text-slate-300"
                  >
                    Username
                  </label>

                  <div className="relative">
                    <AtSign className="absolute left-4 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                    <input
                      id="profile-username"
                      type="text"
                      value={username}
                      onChange={(event) =>
                        setUsername(event.target.value)
                      }
                      placeholder="Username"
                      className="w-full rounded-xl border border-white/10 bg-black/20 py-3.5 pl-11 pr-4 text-sm text-white outline-none placeholder:text-slate-700 focus:border-cyan-400/40"
                    />
                  </div>

                  <p className="mt-2 text-xs text-slate-700">
                    Your username is stored securely in the MorseLab
                    database.
                  </p>
                </div>

                <div className="grid gap-4 sm:grid-cols-2">
                  <div className="rounded-xl border border-white/10 bg-black/20 p-5">
                    <div className="flex items-center gap-3">
                      <CalendarDays className="h-5 w-5 text-cyan-300" />

                      <div>
                        <p className="text-xs text-slate-600">
                          Account Created
                        </p>

                        <p className="mt-1 text-sm font-medium text-slate-300">
                          {formatDate(
                            profile?.created_at || ''
                          )}
                        </p>
                      </div>
                    </div>
                  </div>

                  <div className="rounded-xl border border-white/10 bg-black/20 p-5">
                    <div className="flex items-center gap-3">
                      <Clock3 className="h-5 w-5 text-cyan-300" />

                      <div>
                        <p className="text-xs text-slate-600">
                          Last Login
                        </p>

                        <p className="mt-1 text-sm font-medium text-slate-300">
                          {formatDate(
                            profile?.last_login || ''
                          )}
                        </p>
                      </div>
                    </div>
                  </div>
                </div>

                <div className="rounded-xl border border-cyan-400/10 bg-cyan-400/[0.03] p-5">
                  <div className="flex items-center gap-3">
                    <Activity className="h-5 w-5 text-cyan-300" />

                    <div>
                      <p className="text-xs text-slate-600">
                        Total Translations
                      </p>

                      <p className="mt-1 text-2xl font-bold text-white">
                        {profile?.translation_count ?? 0}
                      </p>
                    </div>
                  </div>
                </div>

                <div className="flex flex-col gap-4 rounded-xl border border-cyan-400/10 bg-cyan-400/[0.03] p-5 sm:flex-row sm:items-start">
                  <ShieldCheck className="mt-0.5 h-5 w-5 shrink-0 text-cyan-300" />

                  <div>
                    <p className="text-sm font-medium text-cyan-300">
                      Account Protection
                    </p>

                    <p className="mt-1 text-xs leading-5 text-slate-600">
                      Your account information is protected through the
                      MorseLab backend authentication and database layer.
                    </p>
                  </div>
                </div>

                <div className="flex flex-col gap-3 border-t border-white/10 pt-6 sm:flex-row sm:items-center sm:justify-between">
                  <div className="flex items-center gap-2 text-xs text-slate-700">
                    <CalendarDays className="h-4 w-4" />
                    Profile information is stored in SQLite.
                  </div>

                  <button
                    type="button"
                    onClick={handleSave}
                    disabled={saving}
                    className="inline-flex items-center justify-center gap-2 rounded-xl bg-cyan-400 px-5 py-3 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300 disabled:cursor-not-allowed disabled:opacity-50"
                  >
                    <Save className="h-4 w-4" />

                    {saving ? 'Saving...' : 'Save Changes'}
                  </button>
                </div>
              </div>
            </section>
          </div>
        )}
      </main>
    </div>
  )
}

export default Profile
