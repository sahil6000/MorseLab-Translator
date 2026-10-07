import {
  Activity,
  ArrowRight,
  BarChart3,
  Clock3,
  History,
  Languages,
  MessageSquareText,
  Plus,
  Save,
  Sparkles,
  TrendingUp,
} from 'lucide-react'
import { Link } from 'react-router-dom'
import { useEffect, useState } from 'react'
import { API_BASE_URL } from '../utils/api'

function Dashboard() {
  const [totalTranslations, setTotalTranslations] = useState(0)
  const [savedTranslations, setSavedTranslations] = useState(0)
  const [weeklyTranslations, setWeeklyTranslations] = useState(0)
  const [, setTextToMorse] = useState(0)
  const [, setMorseToText] = useState(0)
  const [loading, setLoading] = useState(true)
  const [, setError] = useState('')

  const [recentHistory, setRecentHistory] = useState<
  {
    id: number
    input: string
    output: string
    input_type: 'TEXT_TO_MORSE' | 'MORSE_TO_TEXT'
    created_at: string
  }[]
>([])

    useEffect(() => {
    const loadStatistics = async () => {
      const token =
        localStorage.getItem('morselab_token') ||
        sessionStorage.getItem('morselab_token')

      if (!token) {
        setLoading(false)
        return
      }

      try {
        setLoading(true)
        setError('')

        const response = await fetch(
          `${API_BASE_URL}/api/statistics`,
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
            data.error || 'Unable to load statistics.'
          )
        }

        setTotalTranslations(
          data.statistics.total_translations
        )

        setSavedTranslations(
          data.statistics.saved_translations
        )

        setWeeklyTranslations(
          data.statistics.weekly_translations ?? 0
        )

        setTextToMorse(
          data.statistics.text_to_morse
        )

        setMorseToText(
          data.statistics.morse_to_text
        )

const historyResponse = await fetch(
  `${API_BASE_URL}/api/history`,
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

const historyData = await historyResponse.json()

if (!historyResponse.ok || !historyData.success) {
  throw new Error(
    historyData.error || 'Unable to load recent history.'
  )
}

setRecentHistory(
  Array.isArray(historyData.history)
    ? historyData.history.slice(0, 5)
    : []
)

      } catch (error) {
        setError(
          error instanceof Error
            ? error.message
            : 'Unable to load statistics.'
        )
      } finally {
        setLoading(false)
      }
    }

    loadStatistics()
  }, [])

  return (
    <div className="min-h-screen bg-[#050816] text-white">
      {/* Header */}
      <header className="border-b border-white/10 bg-[#070b1a]/90 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-6 py-5">
          <Link to="/" className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <MessageSquareText className="h-5 w-5 text-cyan-300" />
            </div>

            <div>
              <div className="font-bold tracking-widest">MORSELAB</div>
              <div className="text-xs text-slate-500">Translation Platform</div>
            </div>
          </Link>

          <div className="flex items-center gap-3">
            <Link
              to="/profile"
              className="flex h-10 w-10 items-center justify-center rounded-full border border-white/10 bg-white/5 text-sm font-semibold hover:bg-white/10"
            >
              S
            </Link>
          </div>
        </div>
      </header>

      {/* Main */}
      <main className="mx-auto max-w-7xl px-6 py-10">
        {/* Welcome */}
        <section className="mb-10">
          <div className="mb-3 flex items-center gap-2 text-cyan-300">
            <Sparkles className="h-4 w-4" />
            <span className="text-sm font-medium">MorseLab Workspace</span>
          </div>

          <h1 className="text-4xl font-bold tracking-tight md:text-5xl">
            Welcome back.
          </h1>

          <p className="mt-3 max-w-2xl text-slate-400">
            Translate Morse code, manage your translation history, save
            important conversions, and track your activity.
          </p>
        </section>

        {/* Quick Action */}
        <section className="mb-8 rounded-3xl border border-cyan-400/20 bg-gradient-to-br from-cyan-400/10 via-blue-500/5 to-transparent p-8">
          <div className="flex flex-col justify-between gap-8 md:flex-row md:items-center">
            <div>
              <div className="mb-3 flex h-12 w-12 items-center justify-center rounded-2xl bg-cyan-400/10">
                <Languages className="h-6 w-6 text-cyan-300" />
              </div>

              <h2 className="text-2xl font-semibold">
                Start a new translation
              </h2>

              <p className="mt-2 max-w-xl text-sm text-slate-400">
                Convert plain text into Morse code or decode Morse code back
                into readable text.
              </p>
            </div>

            <Link
              to="/translator"
              className="inline-flex items-center justify-center gap-2 rounded-xl bg-cyan-400 px-6 py-3 font-semibold text-slate-950 transition hover:bg-cyan-300"
            >
              Open Translator
              <ArrowRight className="h-4 w-4" />
            </Link>
          </div>
        </section>

        {/* Statistics */}
        <section className="mb-8 grid gap-4 sm:grid-cols-2 lg:grid-cols-4">
          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-5">
            <div className="flex items-center justify-between">
              <span className="text-sm text-slate-400">Translations</span>
              <Languages className="h-5 w-5 text-cyan-300" />
            </div>

            <div className="mt-4 text-3xl font-bold">{loading ? '...' : totalTranslations}</div>
            <p className="mt-1 text-xs text-slate-500">Total conversions</p>
          </div>

          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-5">
            <div className="flex items-center justify-between">
              <span className="text-sm text-slate-400">Saved</span>
              <Save className="h-5 w-5 text-cyan-300" />
            </div>

            <div className="mt-4 text-3xl font-bold">{loading ? '...' : savedTranslations}</div>
            <p className="mt-1 text-xs text-slate-500">Saved translations</p>
          </div>

          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-5">
            <div className="flex items-center justify-between">
              <span className="text-sm text-slate-400">Activity</span>
              <Activity className="h-5 w-5 text-cyan-300" />
            </div>

            <div className="mt-4 text-3xl font-bold">{loading ? '...' : totalTranslations}</div>
            <p className="mt-1 text-xs text-slate-500">Recent operations</p>
          </div>

          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-5">
            <div className="flex items-center justify-between">
              <span className="text-sm text-slate-400">Weekly activity</span>
              <TrendingUp className="h-5 w-5 text-cyan-300" />
            </div>

            <div className="mt-4 text-3xl font-bold">
              {loading ? '...' : weeklyTranslations}
            </div>
            <p className="mt-1 text-xs text-slate-500">
              Translations in the last 7 days
            </p>
          </div>
        </section>

        {/* Two-column section */}
        <section className="grid gap-6 lg:grid-cols-3">
          {/* Recent Activity */}
          <div className="lg:col-span-2 rounded-2xl border border-white/10 bg-white/[0.03]">
            <div className="flex items-center justify-between border-b border-white/10 p-6">
              <div>
                <h2 className="font-semibold">Recent Activity</h2>
                <p className="mt-1 text-xs text-slate-500">
                  Your latest translation operations
                </p>
              </div>

              <Link
                to="/history"
                className="text-sm text-cyan-300 hover:text-cyan-200"
              >
                View all
              </Link>
            </div>

            <div className="p-6">
  {recentHistory.length === 0 ? (
    <div className="flex min-h-[250px] flex-col items-center justify-center text-center">
      <History className="h-10 w-10 text-slate-600" />

      <h3 className="mt-4 font-semibold">
        No translation history
      </h3>

      <p className="mt-2 max-w-sm text-sm text-slate-500">
        Your latest translation operations will appear here after you
        complete your first conversion.
      </p>

      <Link
        to="/translator"
        className="mt-6 inline-flex items-center gap-2 rounded-lg border border-white/10 px-4 py-2 text-sm font-medium transition hover:bg-white/5"
      >
        <Plus className="h-4 w-4" />
        New Translation
      </Link>
    </div>
  ) : (
    <div className="space-y-3">
      {recentHistory.map((item) => (
        <div
          key={item.id}
          className="rounded-xl border border-white/10 bg-white/[0.02] p-4 transition hover:bg-white/[0.04]"
        >
          <div className="flex items-start justify-between gap-4">
            <div className="min-w-0">
              <div className="flex items-center gap-2">
                <History className="h-4 w-4 shrink-0 text-cyan-300" />

                <span className="text-sm font-medium text-white">
                  {item.input_type === 'TEXT_TO_MORSE'
                    ? 'Text → Morse'
                    : 'Morse → Text'}
                </span>
              </div>

              <p className="mt-2 truncate text-sm text-slate-400">
                {item.input}
              </p>

              <p className="mt-1 truncate text-sm text-slate-500">
                {item.output}
              </p>
            </div>

            <span className="shrink-0 text-xs text-slate-600">
              {item.created_at}
            </span>
          </div>
        </div>
      ))}
    </div>
  )}
</div>
</div>

          {/* Quick Links */}
          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-6">
            <h2 className="font-semibold">Quick Access</h2>
            <p className="mt-1 text-xs text-slate-500">
              Manage your MorseLab workspace
            </p>

            <div className="mt-6 space-y-3">
              <Link
                to="/translator"
                className="flex items-center gap-3 rounded-xl border border-white/10 p-4 transition hover:border-cyan-400/30 hover:bg-cyan-400/5"
              >
                <Languages className="h-5 w-5 text-cyan-300" />
                <div className="flex-1">
                  <div className="text-sm font-medium">Translator</div>
                  <div className="text-xs text-slate-500">
                    Translate Morse code
                  </div>
                </div>
                <ArrowRight className="h-4 w-4 text-slate-500" />
              </Link>

              <Link
                to="/history"
                className="flex items-center gap-3 rounded-xl border border-white/10 p-4 transition hover:border-cyan-400/30 hover:bg-cyan-400/5"
              >
                <History className="h-5 w-5 text-cyan-300" />
                <div className="flex-1">
                  <div className="text-sm font-medium">History</div>
                  <div className="text-xs text-slate-500">
                    View translations
                  </div>
                </div>
                <ArrowRight className="h-4 w-4 text-slate-500" />
              </Link>

              <Link
                to="/saved"
                className="flex items-center gap-3 rounded-xl border border-white/10 p-4 transition hover:border-cyan-400/30 hover:bg-cyan-400/5"
              >
                <Save className="h-5 w-5 text-cyan-300" />
                <div className="flex-1">
                  <div className="text-sm font-medium">Saved</div>
                  <div className="text-xs text-slate-500">
                    Saved translations
                  </div>
                </div>
                <ArrowRight className="h-4 w-4 text-slate-500" />
              </Link>

              <Link
                to="/statistics"
                className="flex items-center gap-3 rounded-xl border border-white/10 p-4 transition hover:border-cyan-400/30 hover:bg-cyan-400/5"
              >
                <BarChart3 className="h-5 w-5 text-cyan-300" />
                <div className="flex-1">
                  <div className="text-sm font-medium">Statistics</div>
                  <div className="text-xs text-slate-500">
                    View usage analytics
                  </div>
                </div>
                <ArrowRight className="h-4 w-4 text-slate-500" />
              </Link>
            </div>
          </div>
        </section>

        {/* Footer info */}
        <div className="mt-8 flex items-center gap-2 text-xs text-slate-600">
          <Clock3 className="h-3.5 w-3.5" />
          <span>MorseLab • C-powered translation platform</span>
        </div>
      </main>
    </div>
  )
}

export default Dashboard
