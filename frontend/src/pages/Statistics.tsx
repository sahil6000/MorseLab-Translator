import {
  Activity,
  ArrowLeft,
  BarChart3,
  Clock3,
  Languages,
  TrendingUp,
} from 'lucide-react'
import { Link } from 'react-router-dom'
import { useEffect, useState } from 'react'

type StatisticsData = {
  total_translations: number
  saved_translations: number
  text_to_morse: number
  morse_to_text: number
}

function Statistics() {
  const [statistics, setStatistics] = useState<StatisticsData>({
    total_translations: 0,
    saved_translations: 0,
    text_to_morse: 0,
    morse_to_text: 0,
  })

  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')

  useEffect(() => {
    const loadStatistics = async () => {
      const token =
        localStorage.getItem('morselab_token') ||
        sessionStorage.getItem('morselab_token')

      if (!token) {
        setError('Please log in to view your statistics.')
        setLoading(false)
        return
      }

      try {
        setLoading(true)
        setError('')

        const response = await fetch(
          'http://localhost:8080/api/statistics',
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

        setStatistics(data.statistics)
      } catch (requestError) {
        setError(
          requestError instanceof Error
            ? requestError.message
            : 'Unable to connect to the C backend.'
        )
      } finally {
        setLoading(false)
      }
    }

    loadStatistics()
  }, [])

  const total =
    statistics.text_to_morse +
    statistics.morse_to_text

  const textToMorsePercentage =
    total > 0
      ? Math.round(
          (statistics.text_to_morse / total) * 100
        )
      : 0

  const morseToTextPercentage =
    total > 0
      ? Math.round(
          (statistics.morse_to_text / total) * 100
        )
      : 0

  const savedPercentage =
    statistics.total_translations > 0
      ? Math.min(
          Math.round(
            (statistics.saved_translations /
              statistics.total_translations) *
              100
          ),
          100
        )
      : 0

  const stats = [
    {
      label: 'Total Translations',
      value: statistics.total_translations,
      description: 'All completed translations',
      icon: Languages,
    },
    {
      label: 'Text → Morse',
      value: statistics.text_to_morse,
      description: 'Text converted to Morse',
      icon: Activity,
    },
    {
      label: 'Morse → Text',
      value: statistics.morse_to_text,
      description: 'Morse converted to text',
      icon: TrendingUp,
    },
    {
      label: 'Saved Translations',
      value: statistics.saved_translations,
      description: 'Translations saved for reuse',
      icon: BarChart3,
    },
  ]

  return (
    <div className="min-h-screen bg-[#050816] text-white">
      <header className="border-b border-white/10 bg-[#070b1a]/90 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-6 py-5">
          <Link to="/dashboard" className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <BarChart3 className="h-5 w-5 text-cyan-300" />
            </div>

            <div>
              <div className="font-bold tracking-widest">MORSELAB</div>
              <div className="text-xs text-slate-500">Statistics</div>
            </div>
          </Link>

          <Link
            to="/translator"
            className="rounded-xl bg-cyan-400 px-4 py-2.5 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300"
          >
            New Translation
          </Link>
        </div>
      </header>

      <main className="mx-auto max-w-7xl px-6 py-10">
        <div className="mb-8">
          <Link
            to="/dashboard"
            className="mb-5 inline-flex items-center gap-2 text-sm text-slate-500 transition hover:text-white"
          >
            <ArrowLeft className="h-4 w-4" />
            Dashboard
          </Link>

          <h1 className="text-4xl font-bold tracking-tight">
            Translation Statistics
          </h1>

          <p className="mt-3 max-w-2xl text-slate-400">
            Track your Morse translation activity and usage patterns.
          </p>
        </div>

        {error && (
          <div className="mb-6 rounded-xl border border-red-400/20 bg-red-400/5 px-5 py-4 text-sm text-red-300">
            {error}
          </div>
        )}

        <section className="grid gap-4 sm:grid-cols-2 lg:grid-cols-4">
          {stats.map((stat) => {
            const Icon = stat.icon

            return (
              <div
                key={stat.label}
                className="rounded-2xl border border-white/10 bg-white/[0.03] p-6"
              >
                <div className="mb-5 flex items-center justify-between">
                  <div className="flex h-11 w-11 items-center justify-center rounded-xl bg-cyan-400/10">
                    <Icon className="h-5 w-5 text-cyan-300" />
                  </div>

                  <span className="text-xs uppercase tracking-wider text-slate-700">
                    {loading ? 'Loading' : 'Live'}
                  </span>
                </div>

                <div className="text-3xl font-bold">
                  {loading ? '—' : stat.value}
                </div>

                <div className="mt-2 text-sm font-medium text-slate-300">
                  {stat.label}
                </div>

                <p className="mt-1 text-xs text-slate-600">
                  {stat.description}
                </p>
              </div>
            )
          })}
        </section>

        <section className="mt-6 grid gap-6 lg:grid-cols-2">
          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-6">
            <div className="mb-6 flex items-center gap-3">
              <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-violet-400/10">
                <Clock3 className="h-5 w-5 text-violet-300" />
              </div>

              <div>
                <h2 className="font-semibold">Recent Activity</h2>
                <p className="text-xs text-slate-600">
                  Your latest translation activity
                </p>
              </div>
            </div>

            <div className="flex min-h-[220px] items-center justify-center rounded-xl border border-dashed border-white/10 bg-black/10 px-6 text-center">
              <div>
                {loading ? (
                  <p className="text-sm font-medium text-slate-500">
                    Loading activity...
                  </p>
                ) : statistics.total_translations > 0 ? (
                  <>
                    <p className="text-sm font-medium text-slate-400">
                      Translation activity detected
                    </p>

                    <p className="mt-2 text-xs leading-5 text-slate-700">
                      {statistics.total_translations} total translation
                      {statistics.total_translations === 1 ? '' : 's'} recorded
                      in your account.
                    </p>
                  </>
                ) : (
                  <>
                    <p className="text-sm font-medium text-slate-500">
                      No activity yet
                    </p>

                    <p className="mt-2 text-xs leading-5 text-slate-700">
                      Start translating to generate your activity statistics.
                    </p>
                  </>
                )}
              </div>
            </div>
          </div>

          <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-6">
            <div className="mb-6 flex items-center gap-3">
              <div className="flex h-10 w-10 items-center justify-center rounded-xl bg-cyan-400/10">
                <BarChart3 className="h-5 w-5 text-cyan-300" />
              </div>

              <div>
                <h2 className="font-semibold">Usage Overview</h2>
                <p className="text-xs text-slate-600">
                  Translation distribution
                </p>
              </div>
            </div>

            <div className="space-y-5">
              <div>
                <div className="mb-2 flex items-center justify-between text-sm">
                  <span className="text-slate-400">
                    Text → Morse
                  </span>

                  <span className="text-slate-600">
                    {statistics.text_to_morse}
                  </span>
                </div>

                <div className="h-2 overflow-hidden rounded-full bg-white/5">
                  <div
                    className="h-full rounded-full bg-cyan-400 transition-all"
                    style={{
                      width: `${textToMorsePercentage}%`,
                    }}
                  />
                </div>
              </div>

              <div>
                <div className="mb-2 flex items-center justify-between text-sm">
                  <span className="text-slate-400">
                    Morse → Text
                  </span>

                  <span className="text-slate-600">
                    {statistics.morse_to_text}
                  </span>
                </div>

                <div className="h-2 overflow-hidden rounded-full bg-white/5">
                  <div
                    className="h-full rounded-full bg-violet-400 transition-all"
                    style={{
                      width: `${morseToTextPercentage}%`,
                    }}
                  />
                </div>
              </div>

              <div>
                <div className="mb-2 flex items-center justify-between text-sm">
                  <span className="text-slate-400">
                    Saved
                  </span>

                  <span className="text-slate-600">
                    {statistics.saved_translations}
                  </span>
                </div>

                <div className="h-2 overflow-hidden rounded-full bg-white/5">
                  <div
                    className="h-full rounded-full bg-emerald-400 transition-all"
                    style={{
                      width: `${savedPercentage}%`,
                    }}
                  />
                </div>
              </div>
            </div>
          </div>
        </section>

        <section className="mt-6 rounded-2xl border border-cyan-400/10 bg-cyan-400/[0.03] p-6">
          <div className="flex flex-col gap-5 md:flex-row md:items-center md:justify-between">
            <div>
              <p className="text-sm font-semibold text-cyan-300">
                Live database statistics
              </p>

              <p className="mt-2 max-w-2xl text-sm leading-6 text-slate-500">
                These metrics are loaded directly from your C backend and
                SQLite database.
              </p>
            </div>

            <Link
              to="/translator"
              className="inline-flex shrink-0 items-center justify-center rounded-xl border border-cyan-400/20 bg-cyan-400/10 px-5 py-3 text-sm font-semibold text-cyan-300 transition hover:bg-cyan-400/15"
            >
              Start Translating
            </Link>
          </div>
        </section>
      </main>
    </div>
  )
}

export default Statistics