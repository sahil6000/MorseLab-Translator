import { useState } from 'react'
import type { FormEvent } from 'react'
import {
  ArrowRight,
  CheckCircle2,
  Eye,
  EyeOff,
  LockKeyhole,
  Mail,
  User,
  Wifi,
} from 'lucide-react'
import { Link, useNavigate } from 'react-router-dom'
import { API_BASE_URL } from '../utils/api'

function Signup() {
  const navigate = useNavigate()

  const [name, setName] = useState('')
  const [username, setUsername] = useState('')
  const [email, setEmail] = useState('')
  const [password, setPassword] = useState('')
  const [confirmPassword, setConfirmPassword] = useState('')

  const [showPassword, setShowPassword] = useState(false)
  const [showConfirmPassword, setShowConfirmPassword] = useState(false)

  const [loading, setLoading] = useState(false)
  const [error, setError] = useState('')
  const [success, setSuccess] = useState('')

  const handleSubmit = async (
    event: FormEvent<HTMLFormElement>,
  ) => {
    event.preventDefault()

    setError('')
    setSuccess('')

    const trimmedName = name.trim()
    const trimmedUsername = username.trim()
    const trimmedEmail = email.trim()

    if (
      !trimmedName ||
      !trimmedUsername ||
      !trimmedEmail ||
      !password ||
      !confirmPassword
    ) {
      setError('Please fill in all required fields.')
      return
    }

    if (trimmedName.length < 2) {
      setError('Name must contain at least 2 characters.')
      return
    }

    if (trimmedUsername.length < 3) {
      setError('Username must contain at least 3 characters.')
      return
    }

    if (password.length < 8) {
      setError('Password must contain at least 8 characters.')
      return
    }

    if (password !== confirmPassword) {
      setError('Passwords do not match.')
      return
    }

    setLoading(true)

    try {
      const response = await fetch(
        `${API_BASE_URL}/api/auth/signup`,
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            name: trimmedName,
            username: trimmedUsername,
            email: trimmedEmail,
            password,
          }),
        },
      )

      const data = await response.json()

      if (!response.ok || !data.success) {
        setError(
          data.error ||
            'Unable to create your account. Please try again.',
        )
        return
      }

      setSuccess(
        'Account created successfully. Redirecting to login...',
      )

      setTimeout(() => {
        navigate('/login')
      }, 1200)
    } catch {
      setError(
        'Unable to connect to the MorseLab C backend. Make sure the backend server is running.',
      )
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="min-h-screen bg-[#050816] text-white">
      <div className="grid min-h-screen lg:grid-cols-2">

        {/* Left panel */}
        <section className="relative hidden overflow-hidden border-r border-white/10 lg:flex lg:flex-col lg:justify-between">
          <div className="absolute inset-0 bg-[radial-gradient(circle_at_20%_20%,rgba(34,211,238,0.12),transparent_32%),radial-gradient(circle_at_80%_80%,rgba(59,130,246,0.08),transparent_30%)]" />

          <div className="relative z-10 p-10">
            <Link
              to="/"
              className="inline-flex items-center gap-3"
            >
              <div className="flex h-11 w-11 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
                <Wifi className="h-5 w-5 text-cyan-300" />
              </div>

              <div>
                <div className="font-bold tracking-[0.3em]">
                  MORSELAB
                </div>

                <div className="text-xs text-slate-600">
                  C-Powered Translation Platform
                </div>
              </div>
            </Link>
          </div>

          <div className="relative z-10 max-w-xl p-10 pb-16">
            <p className="mb-4 text-sm font-medium uppercase tracking-[0.25em] text-cyan-300">
              Build your signal
            </p>

            <h1 className="text-5xl font-bold leading-tight tracking-tight">
              Translate.
              <br />
              Decode.
              <br />
              Communicate.
            </h1>

            <p className="mt-6 max-w-lg text-base leading-7 text-slate-400">
              Create your MorseLab account and access a modern
              translation workspace powered by a C-based DSA backend.
            </p>

            <div className="mt-10 grid gap-4">
              {[
                'Real-time Morse translation',
                'Secure account authentication',
                'Translation history and saved results',
              ].map((item) => (
                <div
                  key={item}
                  className="flex items-center gap-3 text-sm text-slate-400"
                >
                  <CheckCircle2 className="h-5 w-5 text-cyan-300" />
                  {item}
                </div>
              ))}
            </div>
          </div>

          <div className="relative z-10 p-10 text-xs text-slate-700">
            MorseLab • C DSA Backend
          </div>
        </section>

        {/* Signup panel */}
        <section className="flex min-h-screen items-center justify-center px-6 py-12">
          <div className="w-full max-w-md">

            <div className="mb-8 lg:hidden">
              <Link
                to="/"
                className="inline-flex items-center gap-3"
              >
                <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
                  <Wifi className="h-5 w-5 text-cyan-300" />
                </div>

                <span className="font-bold tracking-[0.25em]">
                  MORSELAB
                </span>
              </Link>
            </div>

            <div className="mb-8">
              <p className="mb-3 text-sm font-medium uppercase tracking-[0.2em] text-cyan-300">
                Get started
              </p>

              <h2 className="text-3xl font-bold tracking-tight">
                Create your account
              </h2>

              <p className="mt-3 text-sm leading-6 text-slate-500">
                Start translating Morse code with your secure
                workspace.
              </p>
            </div>

            <form
              onSubmit={handleSubmit}
              className="space-y-5"
            >

              {/* Full name */}
              <div>
                <label
                  htmlFor="name"
                  className="mb-2 block text-sm font-medium text-slate-300"
                >
                  Full name
                </label>

                <div className="relative">
                  <User className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                  <input
                    id="name"
                    type="text"
                    value={name}
                    onChange={(event) =>
                      setName(event.target.value)
                    }
                    placeholder="Enter your name"
                    autoComplete="name"
                    disabled={loading}
                    className="w-full rounded-xl border border-white/10 bg-white/[0.03] py-3.5 pl-11 pr-4 text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/40 focus:bg-white/[0.05] disabled:cursor-not-allowed disabled:opacity-60"
                  />
                </div>
              </div>

              {/* Username */}
              <div>
                <label
                  htmlFor="username"
                  className="mb-2 block text-sm font-medium text-slate-300"
                >
                  Username
                </label>

                <div className="relative">
                  <User className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                  <input
                    id="username"
                    type="text"
                    value={username}
                    onChange={(event) =>
                      setUsername(event.target.value)
                    }
                    placeholder="Choose a username"
                    autoComplete="username"
                    disabled={loading}
                    className="w-full rounded-xl border border-white/10 bg-white/[0.03] py-3.5 pl-11 pr-4 text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/40 focus:bg-white/[0.05] disabled:cursor-not-allowed disabled:opacity-60"
                  />
                </div>
              </div>

              {/* Email */}
              <div>
                <label
                  htmlFor="email"
                  className="mb-2 block text-sm font-medium text-slate-300"
                >
                  Email address
                </label>

                <div className="relative">
                  <Mail className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                  <input
                    id="email"
                    type="email"
                    value={email}
                    onChange={(event) =>
                      setEmail(event.target.value)
                    }
                    placeholder="you@example.com"
                    autoComplete="email"
                    disabled={loading}
                    className="w-full rounded-xl border border-white/10 bg-white/[0.03] py-3.5 pl-11 pr-4 text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/40 focus:bg-white/[0.05] disabled:cursor-not-allowed disabled:opacity-60"
                  />
                </div>
              </div>

              {/* Password */}
              <div>
                <label
                  htmlFor="password"
                  className="mb-2 block text-sm font-medium text-slate-300"
                >
                  Password
                </label>

                <div className="relative">
                  <LockKeyhole className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                  <input
                    id="password"
                    type={showPassword ? 'text' : 'password'}
                    value={password}
                    onChange={(event) =>
                      setPassword(event.target.value)
                    }
                    placeholder="Minimum 8 characters"
                    autoComplete="new-password"
                    disabled={loading}
                    className="w-full rounded-xl border border-white/10 bg-white/[0.03] py-3.5 pl-11 pr-12 text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/40 focus:bg-white/[0.05] disabled:cursor-not-allowed disabled:opacity-60"
                  />

                  <button
                    type="button"
                    onClick={() =>
                      setShowPassword((current) => !current)
                    }
                    disabled={loading}
                    className="absolute right-3.5 top-1/2 -translate-y-1/2 text-slate-600 transition hover:text-slate-300"
                    aria-label={
                      showPassword
                        ? 'Hide password'
                        : 'Show password'
                    }
                  >
                    {showPassword ? (
                      <EyeOff className="h-4 w-4" />
                    ) : (
                      <Eye className="h-4 w-4" />
                    )}
                  </button>
                </div>
              </div>

              {/* Confirm password */}
              <div>
                <label
                  htmlFor="confirmPassword"
                  className="mb-2 block text-sm font-medium text-slate-300"
                >
                  Confirm password
                </label>

                <div className="relative">
                  <LockKeyhole className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                  <input
                    id="confirmPassword"
                    type={
                      showConfirmPassword
                        ? 'text'
                        : 'password'
                    }
                    value={confirmPassword}
                    onChange={(event) =>
                      setConfirmPassword(event.target.value)
                    }
                    placeholder="Repeat your password"
                    autoComplete="new-password"
                    disabled={loading}
                    className="w-full rounded-xl border border-white/10 bg-white/[0.03] py-3.5 pl-11 pr-12 text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/40 focus:bg-white/[0.05] disabled:cursor-not-allowed disabled:opacity-60"
                  />

                  <button
                    type="button"
                    onClick={() =>
                      setShowConfirmPassword(
                        (current) => !current,
                      )
                    }
                    disabled={loading}
                    className="absolute right-3.5 top-1/2 -translate-y-1/2 text-slate-600 transition hover:text-slate-300"
                    aria-label={
                      showConfirmPassword
                        ? 'Hide password'
                        : 'Show password'
                    }
                  >
                    {showConfirmPassword ? (
                      <EyeOff className="h-4 w-4" />
                    ) : (
                      <Eye className="h-4 w-4" />
                    )}
                  </button>
                </div>
              </div>

              {/* Error */}
              {error && (
                <div className="rounded-xl border border-red-400/20 bg-red-400/[0.06] px-4 py-3 text-sm leading-5 text-red-300">
                  {error}
                </div>
              )}

              {/* Success */}
              {success && (
                <div className="rounded-xl border border-green-400/20 bg-green-400/[0.06] px-4 py-3 text-sm leading-5 text-green-300">
                  {success}
                </div>
              )}

              {/* Submit */}
              <button
                type="submit"
                disabled={loading}
                className="group flex w-full items-center justify-center gap-2 rounded-xl bg-cyan-400 px-5 py-3.5 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300 disabled:cursor-not-allowed disabled:opacity-60"
              >
                {loading ? (
                  <>
                    <span className="h-4 w-4 animate-spin rounded-full border-2 border-slate-950/30 border-t-slate-950" />
                    Creating account...
                  </>
                ) : (
                  <>
                    Create account
                    <ArrowRight className="h-4 w-4 transition group-hover:translate-x-0.5" />
                  </>
                )}
              </button>
            </form>

            <p className="mt-7 text-center text-sm text-slate-500">
              Already have an account?{' '}
              <Link
                to="/login"
                className="font-medium text-cyan-300 transition hover:text-cyan-200"
              >
                Sign in
              </Link>
            </p>

            <p className="mt-8 text-center text-xs leading-5 text-slate-700">
              Your password is processed by the MorseLab C backend
              and stored as a secure derived hash.
            </p>
          </div>
        </section>
      </div>
    </div>
  )
}

export default Signup
