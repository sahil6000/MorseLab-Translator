import { ArrowLeft, Binary, Eye, EyeOff, Lock, Mail } from 'lucide-react'
import { useState } from 'react'
import type { FormEvent } from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { notifyActivityIfEnabled } from '../utils/notifications'

function Login() {
  const navigate = useNavigate()

  const [email, setEmail] = useState('')
  const [password, setPassword] = useState('')
  const [showPassword, setShowPassword] = useState(false)
  const [rememberMe, setRememberMe] = useState(false)

  const [loading, setLoading] = useState(false)
  const [error, setError] = useState('')

  const handleSubmit = async (
    event: FormEvent<HTMLFormElement>
  ) => {
    event.preventDefault()

    setError('')

    const trimmedEmail = email.trim()

    if (!trimmedEmail || !password) {
      setError('Please enter your email and password.')
      return
    }

    setLoading(true)

    try {
      const response = await fetch(
        'http://localhost:8080/api/auth/login',
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            email: trimmedEmail,
            password,
          }),
        }
      )

      const data = await response.json()

      if (!response.ok || !data.success) {
        setError(
          data.error ||
            'Invalid email or password.'
        )
        return
      }

      if (!data.token || !data.user) {
        setError(
          'Login succeeded, but the authentication session was not created.'
        )
        return
      }

      const storage = rememberMe
        ? localStorage
        : sessionStorage

      storage.setItem(
        'morselab_token',
        data.token
      )

      storage.setItem(
        'morselab_user',
        JSON.stringify(data.user)
      )

      void notifyActivityIfEnabled(
        data.token,
        'MorseLab Account Activity',
        'A new login to your account was completed.',
      )

      navigate('/dashboard')
    } catch {
      setError(
        'Unable to connect to the MorseLab C backend. Make sure the backend server is running.'
      )
    } finally {
      setLoading(false)
    }
  }

  return (
    <div className="min-h-screen bg-[#050816] text-white">
      <div className="mx-auto flex min-h-screen max-w-7xl">

        <div className="hidden w-1/2 flex-col justify-between border-r border-white/10 p-10 lg:flex">
          <Link
            to="/"
            className="flex items-center gap-3"
          >
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <Binary className="h-5 w-5 text-cyan-300" />
            </div>

            <span className="font-bold tracking-wide">
              MORSELAB
            </span>
          </Link>

          <div>
            <p className="text-sm uppercase tracking-[0.25em] text-cyan-300">
              Secure Workspace
            </p>

            <h1 className="mt-5 max-w-lg text-5xl font-bold leading-tight">
              Your translation workspace,
              <span className="text-cyan-300">
                {' '}connected.
              </span>
            </h1>

            <p className="mt-6 max-w-md leading-7 text-slate-400">
              Access your translations, saved messages,
              history, statistics, and MorseLab tools
              from one workspace.
            </p>
          </div>

          <p className="text-sm text-slate-600">
            Pure C backend · SQLite · Real DSA
          </p>
        </div>

        <div className="flex w-full items-center justify-center px-6 py-12 lg:w-1/2">

          <div className="w-full max-w-md">

            <Link
              to="/"
              className="mb-8 inline-flex items-center gap-2 text-sm text-slate-500 transition hover:text-white"
            >
              <ArrowLeft className="h-4 w-4" />
              Back to home
            </Link>

            <div className="mb-8 lg:hidden">
              <div className="flex items-center gap-3">

                <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
                  <Binary className="h-5 w-5 text-cyan-300" />
                </div>

                <span className="font-bold tracking-wide">
                  MORSELAB
                </span>

              </div>
            </div>

            <div className="rounded-2xl border border-white/10 bg-white/[0.025] p-7 shadow-2xl shadow-black/20">

              <div>
                <p className="text-sm font-semibold uppercase tracking-[0.2em] text-cyan-300">
                  Welcome back
                </p>

                <h2 className="mt-3 text-3xl font-bold">
                  Sign in
                </h2>

                <p className="mt-2 text-sm text-slate-500">
                  Enter your credentials to access your workspace.
                </p>
              </div>

              {error && (
                <div className="mt-5 rounded-xl border border-red-400/20 bg-red-400/10 px-4 py-3 text-sm text-red-300">
                  {error}
                </div>
              )}

              <form
                onSubmit={handleSubmit}
                className="mt-8 space-y-5"
              >

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
                      name="email"
                      type="email"
                      value={email}
                      onChange={(event) =>
                        setEmail(event.target.value)
                      }
                      required
                      placeholder="you@example.com"
                      disabled={loading}
                      className="w-full rounded-xl border border-white/10 bg-black/20 py-3 pl-10 pr-4 text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/50 focus:ring-2 focus:ring-cyan-400/10 disabled:cursor-not-allowed disabled:opacity-60"
                    />

                  </div>
                </div>

                <div>
                  <label
                    htmlFor="password"
                    className="mb-2 block text-sm font-medium text-slate-300"
                  >
                    Password
                  </label>

                  <div className="relative">

                    <Lock className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

                    <input
                      id="password"
                      name="password"
                      type={
                        showPassword
                          ? 'text'
                          : 'password'
                      }
                      value={password}
                      onChange={(event) =>
                        setPassword(event.target.value)
                      }
                      required
                      placeholder="Enter your password"
                      disabled={loading}
                      className="w-full rounded-xl border border-white/10 bg-black/20 py-3 pl-10 pr-11 text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/50 focus:ring-2 focus:ring-cyan-400/10 disabled:cursor-not-allowed disabled:opacity-60"
                    />

                    <button
                      type="button"
                      onClick={() =>
                        setShowPassword(!showPassword)
                      }
                      disabled={loading}
                      className="absolute right-3.5 top-1/2 -translate-y-1/2 text-slate-600 transition hover:text-slate-300 disabled:opacity-50"
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

                <div className="flex items-center justify-between text-sm">

                  <label className="flex items-center gap-2 text-slate-500">

                    <input
                      type="checkbox"
                      checked={rememberMe}
                      onChange={(event) =>
                        setRememberMe(event.target.checked)
                      }
                      disabled={loading}
                      className="h-4 w-4 rounded border-white/10 bg-black/20 accent-cyan-400"
                    />

                    Remember me

                  </label>

                  <button
                    type="button"
                    className="text-cyan-300 transition hover:text-cyan-200"
                  >
                    Forgot password?
                  </button>

                </div>

                <button
                  type="submit"
                  disabled={loading}
                  className="w-full rounded-xl bg-cyan-400 py-3.5 font-semibold text-slate-950 transition hover:bg-cyan-300 disabled:cursor-not-allowed disabled:opacity-60"
                >
                  {loading
                    ? 'Signing in...'
                    : 'Sign In'}
                </button>

              </form>

              <p className="mt-7 text-center text-sm text-slate-500">
                Don't have an account?{' '}

                <Link
                  to="/signup"
                  className="font-medium text-cyan-300 hover:text-cyan-200"
                >
                  Create one
                </Link>
              </p>

            </div>
          </div>
        </div>
      </div>
    </div>
  )
}

export default Login
