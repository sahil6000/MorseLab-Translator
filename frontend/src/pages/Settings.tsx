import {
  ArrowLeft,
  Bell,
  CheckCircle2,
  Database,
  KeyRound,
  LogOut,
  Save,
  ShieldCheck,
  Trash2,
  UserCog,
  XCircle,
} from 'lucide-react'
import { useEffect, useState } from 'react'
import {
  requestNotificationPermission,
  notifyActivityIfEnabled,
  showActivityNotification,
} from '../utils/notifications'
import { Link, useNavigate } from 'react-router-dom'
import { API_BASE_URL } from '../utils/api'

function Settings() {
  const navigate = useNavigate()

  const [notifications, setNotifications] = useState(false)
  const [notificationsLoading, setNotificationsLoading] = useState(true)
  const [notificationsError, setNotificationsError] = useState('')
  const [compactMode, setCompactMode] = useState(() => {
    return localStorage.getItem('morselab_compact_mode') === 'true'
  })

  useEffect(() => {
    document.documentElement.classList.toggle(
      'compact-mode',
      compactMode,
    )
  }, [compactMode])

  const [deleteAccountLoading, setDeleteAccountLoading] = useState(false)
  const [deleteAccountError, setDeleteAccountError] = useState('')

  const [showPasswordForm, setShowPasswordForm] = useState(false)

  const [currentPassword, setCurrentPassword] = useState('')
  const [newPassword, setNewPassword] = useState('')
  const [confirmPassword, setConfirmPassword] = useState('')

  const [passwordLoading, setPasswordLoading] = useState(false)
  const [passwordMessage, setPasswordMessage] = useState('')
  const [passwordError, setPasswordError] = useState('')

  const [saveMessage, setSaveMessage] = useState('')

  const getToken = () => {
    return (
      localStorage.getItem('morselab_token') ||
      sessionStorage.getItem('morselab_token') ||
      ''
    )
  }

  useEffect(() => {
    let active = true
    const token = getToken()

    if (!token) {
      setNotificationsLoading(false)
      return
    }

    fetch(`${API_BASE_URL}/api/preferences`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ token }),
    })
      .then(async (response) => {
        const data = await response.json()
        if (!response.ok || !data.success) {
          throw new Error(data.error || 'Unable to load preferences.')
        }

        if (active) {
          setNotifications(
            data.preferences?.activity_notifications_enabled === 1,
          )
          setNotificationsError('')
        }
      })
      .catch((error: unknown) => {
        if (active) {
          setNotificationsError(
            error instanceof Error
              ? error.message
              : 'Unable to load notification preferences.',
          )
        }
      })
      .finally(() => {
        if (active) {
          setNotificationsLoading(false)
        }
      })

    return () => {
      active = false
    }
  }, [])

  const handleSave = () => {
    localStorage.setItem(
      'morselab_compact_mode',
      String(compactMode),
    )

    document.documentElement.classList.toggle('compact-mode', compactMode)

    setSaveMessage('Preferences saved successfully.')

    window.setTimeout(() => {
      setSaveMessage('')
    }, 3000)
  }

  const handleNotificationToggle = async () => {
    const nextValue = !notifications
    setNotificationsError('')

    if (nextValue) {
      const permissionGranted = await requestNotificationPermission()
      if (!permissionGranted) {
        setNotificationsError(
          'Browser notification permission is required to enable this preference.',
        )
        return
      }
    }

    const token = getToken()
    if (!token) {
      setNotificationsError('Your session has expired. Please log in again.')
      return
    }

    setNotificationsLoading(true)
    try {
      const response = await fetch(`${API_BASE_URL}/api/preferences`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          token,
          activity_notifications_enabled: nextValue ? 1 : 0,
        }),
      })
      const data = await response.json()

      if (!response.ok || !data.success) {
        throw new Error(data.error || 'Unable to save notification preference.')
      }

      setNotifications(
        data.preferences?.activity_notifications_enabled === 1,
      )

      if (nextValue) {
        showActivityNotification(
          'MorseLab Notifications Enabled',
          'You will now receive important activity notifications.',
        )
      }
    } catch (error) {
      setNotificationsError(
        error instanceof Error
          ? error.message
          : 'Unable to save notification preference.',
      )
    } finally {
      setNotificationsLoading(false)
    }
  }

  const handleChangePassword = async () => {
    setPasswordMessage('')
    setPasswordError('')

    if (!currentPassword || !newPassword || !confirmPassword) {
      setPasswordError('Please fill in all password fields.')
      return
    }

    if (newPassword.length < 8) {
      setPasswordError(
        'New password must be at least 8 characters long.',
      )
      return
    }

    if (newPassword !== confirmPassword) {
      setPasswordError(
        'New password and confirmation password do not match.',
      )
      return
    }

    if (currentPassword === newPassword) {
      setPasswordError(
        'New password must be different from your current password.',
      )
      return
    }

    const token = getToken()

    if (!token) {
      setPasswordError(
        'Your session has expired. Please log in again.',
      )
      return
    }

    setPasswordLoading(true)

    try {
      const response = await fetch(
        `${API_BASE_URL}/api/auth/change-password`,
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            token,
            current_password: currentPassword,
            new_password: newPassword,
          }),
        },
      )

      const data = await response.json()

      if (!response.ok || !data.success) {
        setPasswordError(
          data.error ||
            'Unable to change password. Please check your current password.',
        )
        return
      }

      setCurrentPassword('')
      setNewPassword('')
      setConfirmPassword('')

      setPasswordMessage(
        'Password changed successfully.',
      )

      void notifyActivityIfEnabled(
        token,
        'MorseLab Account Security',
        'Your account password was changed.',
      )

      window.setTimeout(() => {
        setPasswordMessage('')
      }, 4000)
    } catch {
      setPasswordError(
        'Unable to connect to the MorseLab backend.',
      )
    } finally {
      setPasswordLoading(false)
    }
  }

  const handleLogout = async () => {
    const token = getToken()

    try {
      if (token) {
        await fetch(
          `${API_BASE_URL}/api/auth/logout`,
          {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify({
              token,
            }),
          },
        )
      }
    } catch {
      // Continue logout locally even if the backend is unavailable.
    }

    localStorage.removeItem('morselab_token')
    localStorage.removeItem('morselab_user')

    sessionStorage.removeItem('morselab_token')
    sessionStorage.removeItem('morselab_user')

    navigate('/login')
  }

  const handleDeleteAccount = async () => {
  const confirmed = window.confirm(
    'Are you sure you want to delete your account? This will permanently remove your account, translation history, and saved translations.'
  )

  if (!confirmed) {
    return
  }

  const token = getToken()

  if (!token) {
    setDeleteAccountError('You are not logged in.')
    return
  }

  setDeleteAccountLoading(true)
  setDeleteAccountError('')

  try {
    const response = await fetch(
      `${API_BASE_URL}/api/profile/delete`,
      {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          token,
        }),
      }
    )

    const data = await response.json()

    if (!response.ok || !data.success) {
      throw new Error(
        data.error || 'Unable to delete your account.'
      )
    }

    localStorage.removeItem('morselab_token')
    localStorage.removeItem('morselab_user')
    sessionStorage.removeItem('morselab_token')
    sessionStorage.removeItem('morselab_user')

    navigate('/login')
  } catch (deleteError) {
    setDeleteAccountError(
      deleteError instanceof Error
        ? deleteError.message
        : 'Unable to delete your account.'
    )
  } finally {
    setDeleteAccountLoading(false)
  }
}

  return (
    <div className="min-h-screen bg-[#050816] text-white">
      <header className="border-b border-white/10 bg-[#070b1a]/90 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-6 py-5">
          <Link
            to="/dashboard"
            className="flex items-center gap-3"
          >
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <UserCog className="h-5 w-5 text-cyan-300" />
            </div>

            <div>
              <div className="font-bold tracking-widest">
                MORSELAB
              </div>

              <div className="text-xs text-slate-500">
                Settings
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
            Application Settings
          </h1>

          <p className="mt-3 max-w-2xl text-slate-400">
            Control your MorseLab preferences, security options,
            and account behavior.
          </p>
        </div>

        <div className="space-y-6">
          {/* Preferences */}
          <section className="rounded-2xl border border-white/10 bg-white/[0.03] p-6 md:p-8">
            <div className="mb-7 flex items-start gap-4">
              <div className="flex h-11 w-11 shrink-0 items-center justify-center rounded-xl bg-cyan-400/10">
                <Bell className="h-5 w-5 text-cyan-300" />
              </div>

              <div>
                <h2 className="font-semibold">
                  Preferences
                </h2>

                <p className="mt-1 text-sm text-slate-600">
                  Customize how MorseLab behaves for your account.
                </p>
              </div>
            </div>

              {notificationsError && (
                <p className="mb-4 text-sm text-amber-300" role="status">
                  {notificationsError}
                </p>
              )}

              <div className="divide-y divide-white/10">
              <div className="flex items-center justify-between gap-6 py-5 first:pt-0">
                <div>
                  <p className="text-sm font-medium text-slate-300">
                    Activity Notifications
                  </p>

                  <p className="mt-1 text-xs leading-5 text-slate-600">
                    Receive notifications about important account activity.
                  </p>
                </div>

                <button
                  type="button"
                  onClick={handleNotificationToggle}
                  disabled={notificationsLoading}
                  aria-pressed={notifications}
                  className={`relative h-6 w-11 shrink-0 rounded-full transition ${
                    notifications
                      ? 'bg-cyan-400'
                      : 'bg-white/10'
                  } disabled:cursor-not-allowed disabled:opacity-50`}
                  aria-label="Toggle activity notifications"
                >
                  <span
                    className={`absolute top-1 h-4 w-4 rounded-full bg-white transition ${
                      notifications
                        ? 'left-6'
                        : 'left-1'
                    }`}
                  />
                </button>
              </div>

              <div className="flex items-center justify-between gap-6 py-5 last:pb-0">
                <div>
                  <p className="text-sm font-medium text-slate-300">
                    Compact Interface
                  </p>

                  <p className="mt-1 text-xs leading-5 text-slate-600">
                    Use a denser interface for translation and history views.
                  </p>
                </div>

                <button
                  type="button"
                  onClick={() => {
                    const nextValue = !compactMode
                    setCompactMode(nextValue)
                    localStorage.setItem(
                      'morselab_compact_mode',
                      String(nextValue),
                    )
                  }}
                  aria-pressed={compactMode}
                  className={`relative h-6 w-11 shrink-0 rounded-full transition ${
                    compactMode
                      ? 'bg-violet-400'
                      : 'bg-white/10'
                  }`}
                  aria-label="Toggle compact interface"
                >
                  <span
                    className={`absolute top-1 h-4 w-4 rounded-full bg-white transition ${
                      compactMode
                        ? 'left-6'
                        : 'left-1'
                    }`}
                  />
                </button>
              </div>
            </div>
          </section>

          {/* Security */}
          <section className="rounded-2xl border border-white/10 bg-white/[0.03] p-6 md:p-8">
            <div className="mb-7 flex items-start gap-4">
              <div className="flex h-11 w-11 shrink-0 items-center justify-center rounded-xl bg-violet-400/10">
                <ShieldCheck className="h-5 w-5 text-violet-300" />
              </div>

              <div>
                <h2 className="font-semibold">
                  Security
                </h2>

                <p className="mt-1 text-sm text-slate-600">
                  Manage authentication and account protection.
                </p>
              </div>
            </div>

            <div className="space-y-4">
              <button
                type="button"
                onClick={() =>
                  setShowPasswordForm(!showPasswordForm)
                }
                className="flex w-full items-center justify-between rounded-xl border border-white/10 bg-black/10 p-4 text-left transition hover:bg-white/5"
              >
                <div className="flex items-center gap-4">
                  <KeyRound className="h-5 w-5 text-slate-500" />

                  <div>
                    <p className="text-sm font-medium text-slate-300">
                      Change Password
                    </p>

                    <p className="mt-1 text-xs text-slate-700">
                      Update your account password securely.
                    </p>
                  </div>
                </div>

                <span className="text-xs text-cyan-300">
                  {showPasswordForm ? 'Close' : 'Manage'}
                </span>
              </button>

              {showPasswordForm && (
                <div className="rounded-xl border border-violet-400/10 bg-violet-400/[0.03] p-5">
                  <div className="mb-5">
                    <h3 className="text-sm font-semibold text-slate-200">
                      Change Account Password
                    </h3>

                    <p className="mt-1 text-xs leading-5 text-slate-600">
                      Enter your current password and choose a new
                      password for your account.
                    </p>
                  </div>

                  {passwordMessage && (
                    <div className="mb-4 flex items-center gap-2 rounded-xl border border-emerald-400/20 bg-emerald-400/5 px-4 py-3 text-sm text-emerald-300">
                      <CheckCircle2 className="h-4 w-4 shrink-0" />
                      {passwordMessage}
                    </div>
                  )}

                  {passwordError && (
                    <div className="mb-4 flex items-center gap-2 rounded-xl border border-red-400/20 bg-red-400/5 px-4 py-3 text-sm text-red-300">
                      <XCircle className="h-4 w-4 shrink-0" />
                      {passwordError}
                    </div>
                  )}

                  <div className="space-y-4">
                    <div>
                      <label
                        htmlFor="current-password"
                        className="mb-2 block text-xs font-medium text-slate-300"
                      >
                        Current Password
                      </label>

                      <input
                        id="current-password"
                        type="password"
                        value={currentPassword}
                        onChange={(event) =>
                          setCurrentPassword(
                            event.target.value,
                          )
                        }
                        placeholder="Enter current password"
                        className="w-full rounded-xl border border-white/10 bg-black/20 px-4 py-3 text-sm text-white outline-none placeholder:text-slate-700 focus:border-violet-400/40"
                      />
                    </div>

                    <div>
                      <label
                        htmlFor="new-password"
                        className="mb-2 block text-xs font-medium text-slate-300"
                      >
                        New Password
                      </label>

                      <input
                        id="new-password"
                        type="password"
                        value={newPassword}
                        onChange={(event) =>
                          setNewPassword(
                            event.target.value,
                          )
                        }
                        placeholder="Enter new password"
                        className="w-full rounded-xl border border-white/10 bg-black/20 px-4 py-3 text-sm text-white outline-none placeholder:text-slate-700 focus:border-violet-400/40"
                      />
                    </div>

                    <div>
                      <label
                        htmlFor="confirm-password"
                        className="mb-2 block text-xs font-medium text-slate-300"
                      >
                        Confirm New Password
                      </label>

                      <input
                        id="confirm-password"
                        type="password"
                        value={confirmPassword}
                        onChange={(event) =>
                          setConfirmPassword(
                            event.target.value,
                          )
                        }
                        placeholder="Confirm new password"
                        className="w-full rounded-xl border border-white/10 bg-black/20 px-4 py-3 text-sm text-white outline-none placeholder:text-slate-700 focus:border-violet-400/40"
                      />
                    </div>

                    <button
                      type="button"
                      onClick={handleChangePassword}
                      disabled={passwordLoading}
                      className="inline-flex items-center justify-center gap-2 rounded-xl bg-violet-400 px-5 py-3 text-sm font-semibold text-slate-950 transition hover:bg-violet-300 disabled:cursor-not-allowed disabled:opacity-50"
                    >
                      <KeyRound className="h-4 w-4" />

                      {passwordLoading
                        ? 'Updating Password...'
                        : 'Update Password'}
                    </button>
                  </div>
                </div>
              )}

              <div className="flex items-center gap-4 rounded-xl border border-emerald-400/10 bg-emerald-400/[0.03] p-4">
                <ShieldCheck className="h-5 w-5 text-emerald-400" />

                <div>
                  <p className="text-sm font-medium text-emerald-300">
                    Protected Account
                  </p>

                  <p className="mt-1 text-xs text-slate-600">
                    Authentication is handled through the secure C
                    backend and SQLite database layer.
                  </p>
                </div>
              </div>
            </div>
          </section>

          {/* Delete Account */}
<section className="rounded-2xl border border-red-400/20 bg-red-400/[0.03] p-6 md:p-8">
  <div className="mb-6 flex items-start gap-4">
    <div className="flex h-11 w-11 shrink-0 items-center justify-center rounded-xl bg-red-400/10">
      <Trash2 className="h-5 w-5 text-red-300" />
    </div>

    <div>
      <h2 className="font-semibold text-red-300">
        Delete Account
      </h2>

      <p className="mt-1 text-sm text-slate-500">
        Permanently delete your MorseLab account and associated data.
      </p>
    </div>
  </div>

  <div className="rounded-xl border border-red-400/10 bg-black/10 p-4">
    <p className="text-sm leading-6 text-slate-400">
      This action permanently removes your account, translation history,
      and saved translations. This action cannot be undone.
    </p>

    {deleteAccountError && (
      <p className="mt-3 text-sm text-red-300">
        {deleteAccountError}
      </p>
    )}

    <button
      type="button"
      onClick={handleDeleteAccount}
      disabled={deleteAccountLoading}
      className="mt-5 inline-flex items-center justify-center gap-2 rounded-xl border border-red-400/20 bg-red-400/10 px-5 py-3 text-sm font-semibold text-red-300 transition hover:bg-red-400/20 disabled:cursor-not-allowed disabled:opacity-50"
    >
      <Trash2 className="h-4 w-4" />

      {deleteAccountLoading
        ? 'Deleting Account...'
        : 'Delete Account'}
    </button>
  </div>
</section>

          {/* Data & Privacy */}
          <section className="rounded-2xl border border-white/10 bg-white/[0.03] p-6 md:p-8">
            <div className="mb-7 flex items-start gap-4">
              <div className="flex h-11 w-11 shrink-0 items-center justify-center rounded-xl bg-amber-400/10">
                <Database className="h-5 w-5 text-amber-300" />
              </div>

              <div>
                <h2 className="font-semibold">
                  Data & Privacy
                </h2>

                <p className="mt-1 text-sm text-slate-600">
                  Understand how your translation data is managed.
                </p>
              </div>
            </div>

            <div className="space-y-3">
              <div className="rounded-xl border border-white/10 bg-black/10 p-4">
                <p className="text-sm font-medium text-slate-300">
                  Translation History
                </p>

                <p className="mt-1 text-xs leading-5 text-slate-600">
                  Your translation history is associated with your
                  account and stored in the application database.
                </p>
              </div>

              <div className="rounded-xl border border-white/10 bg-black/10 p-4">
                <p className="text-sm font-medium text-slate-300">
                  Saved Translations
                </p>

                <p className="mt-1 text-xs leading-5 text-slate-600">
                  Saved translations can be reviewed and managed from
                  the Saved section.
                </p>
              </div>
            </div>
          </section>

          {/* Bottom actions */}
          <div className="flex flex-col gap-3 border-t border-white/10 pt-6 sm:flex-row sm:items-center sm:justify-between">
            <button
              type="button"
              onClick={handleLogout}
              className="inline-flex items-center justify-center gap-2 rounded-xl border border-red-400/20 bg-red-400/5 px-5 py-3 text-sm font-semibold text-red-300 transition hover:bg-red-400/10"
            >
              <LogOut className="h-4 w-4" />
              Logout
            </button>

            <div className="flex items-center gap-3">
              {saveMessage && (
                <span className="text-xs text-emerald-300">
                  {saveMessage}
                </span>
              )}

              <button
                type="button"
                onClick={handleSave}
                className="inline-flex items-center justify-center gap-2 rounded-xl bg-cyan-400 px-5 py-3 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300"
              >
                <Save className="h-4 w-4" />
                Save Preferences
              </button>
            </div>
          </div>
        </div>
      </main>
    </div>
  )
}

export default Settings
