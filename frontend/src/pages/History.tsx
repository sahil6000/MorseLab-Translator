import {
  ArrowLeft,
  CalendarDays,
  Check,
  ChevronDown,
  Copy,
  History as HistoryIcon,
  Search,
  Trash2,
} from 'lucide-react'
import { useEffect, useState } from 'react'
import { Link } from 'react-router-dom'

type HistoryItem = {
  id: number
  type: 'TEXT_TO_MORSE' | 'MORSE_TO_TEXT'
  input: string
  output: string
  date: string
}

type BackendHistoryItem = {
  id: number
  input: string
  output: string
  input_type: 'TEXT_TO_MORSE' | 'MORSE_TO_TEXT'
  created_at: string
}

function formatHistoryDate(dateString: string): string {
  const utcDate = new Date(
    dateString.replace(' ', 'T') + 'Z'
  )

  return utcDate.toLocaleString('en-IN', {
    timeZone: 'Asia/Kolkata',
    year: 'numeric',
    month: '2-digit',
    day: '2-digit',
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit',
    hour12: false,
  })
}

function History() {
  const [search, setSearch] = useState('')
  const [filter, setFilter] = useState('ALL')
  const [copiedId, setCopiedId] = useState<number | null>(null)

  const [history, setHistory] = useState<HistoryItem[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')

  const [compactMode] = useState(() => {
  return localStorage.getItem('morselab_compact_mode') === 'true'
})

  /*
   * =====================================================
   * LOAD TRANSLATION HISTORY FROM C BACKEND
   * =====================================================
   */

  useEffect(() => {
    const token =
      localStorage.getItem('morselab_token') ||
      sessionStorage.getItem('morselab_token')

    if (!token) {
      setError('Your session has expired. Please login again.')
      setLoading(false)
      return
    }

    const loadHistory = async () => {
      try {
        setLoading(true)
        setError('')

        const response = await fetch(
          'http://localhost:8080/api/history',
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
            data.error || 'Failed to load translation history.'
          )
        }

        const backendHistory: BackendHistoryItem[] =
          Array.isArray(data.history)
            ? data.history
            : []

        const historyItems: HistoryItem[] =
          backendHistory.map((item) => ({
            id: item.id,
            type: item.input_type,
            input: item.input,
            output: item.output,
            date: formatHistoryDate(item.created_at),
          }))

        setHistory(historyItems)
      } catch (requestError) {
        setError(
          requestError instanceof Error
            ? requestError.message
            : 'Failed to load translation history.'
        )
      } finally {
        setLoading(false)
      }
    }

    loadHistory()
  }, [])

  /*
   * =====================================================
   * SEARCH + FILTER
   * =====================================================
   */

  const filteredHistory = history.filter((item) => {
    const searchValue = search.toLowerCase()

    const matchesSearch =
      item.input.toLowerCase().includes(searchValue) ||
      item.output.toLowerCase().includes(searchValue)

    const matchesFilter =
      filter === 'ALL' || item.type === filter

    return matchesSearch && matchesFilter
  })

  /*
   * =====================================================
   * COPY TRANSLATION
   * =====================================================
   */

  const copyTranslation = async (item: HistoryItem) => {
    try {
      await navigator.clipboard.writeText(item.output)

      setCopiedId(item.id)

      setTimeout(() => {
        setCopiedId(null)
      }, 1500)
    } catch {
      setError('Unable to copy the translation.')
    }
  }

/*
 * =====================================================
 * DELETE TRANSLATION
 * =====================================================
 */

const deleteTranslation = async (historyId: number) => {
  const token =
    localStorage.getItem('morselab_token') ||
    sessionStorage.getItem('morselab_token')

  if (!token) {
    setError('Your session has expired. Please login again.')
    return
  }

  try {
    setError('')

    const response = await fetch(
      'http://localhost:8080/api/history',
      {
        method: 'DELETE',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          token,
          history_id: historyId,
        }),
      }
    )

    const data = await response.json()

    if (!response.ok || !data.success) {
      throw new Error(
        data.error || 'Failed to delete translation.'
      )
    }

    setHistory((currentHistory) =>
      currentHistory.filter(
        (item) => item.id !== historyId
      )
    )
  } catch (requestError) {
    setError(
      requestError instanceof Error
        ? requestError.message
        : 'Failed to delete translation.'
    )
  }
}

  /*
   * =====================================================
   * PAGE UI
   * =====================================================
   */

  return (
  <div
    className={`min-h-screen bg-[#050816] text-white ${
      compactMode ? 'compact-history' : ''
    }`}
  >

      {/* =================================================
          HEADER
      ================================================= */}

      <header className="border-b border-white/10 bg-[#070b1a]/90 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-6 py-5">

          <Link
            to="/dashboard"
            className="flex items-center gap-3"
          >
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <HistoryIcon className="h-5 w-5 text-cyan-300" />
            </div>

            <div>
              <div className="font-bold tracking-widest">
                MORSELAB
              </div>

              <div className="text-xs text-slate-500">
                History
              </div>
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

      {/* =================================================
          MAIN CONTENT
      ================================================= */}

      <main
        className={`mx-auto max-w-7xl px-6 ${
          compactMode ? 'py-6' : 'py-10'
      }`}
   >

        {/* =================================================
            HEADING
        ================================================= */}

        <div className="mb-8">

          <Link
            to="/dashboard"
            className="mb-5 inline-flex items-center gap-2 text-sm text-slate-500 transition hover:text-white"
          >
            <ArrowLeft className="h-4 w-4" />
            Dashboard
          </Link>

          <h1 className="text-4xl font-bold tracking-tight">
            Translation History
          </h1>

          <p className="mt-3 max-w-2xl text-slate-400">
            Review your previous Morse code translations and quickly reuse
            their results.
          </p>

        </div>

        {/* =================================================
            SEARCH + FILTER CONTROLS
        ================================================= */}

        <section className="mb-6 flex flex-col gap-3 rounded-2xl border border-white/10 bg-white/[0.03] p-4 md:flex-row">

          {/* Search */}

          <div className="relative flex-1">

            <Search className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

            <input
              type="text"
              value={search}
              onChange={(event) =>
                setSearch(event.target.value)
              }
              placeholder="Search translations..."
              className="w-full rounded-xl border border-white/10 bg-black/20 py-3 pl-10 pr-4 text-sm text-white outline-none placeholder:text-slate-700 focus:border-cyan-400/40"
            />

          </div>

          {/* Filter */}

          <div className="relative">

            <select
              value={filter}
              onChange={(event) =>
                setFilter(event.target.value)
              }
              className="appearance-none rounded-xl border border-white/10 bg-black/20 py-3 pl-4 pr-10 text-sm text-slate-300 outline-none focus:border-cyan-400/40"
            >
              <option value="ALL">
                All translations
              </option>

              <option value="TEXT_TO_MORSE">
                Text → Morse
              </option>

              <option value="MORSE_TO_TEXT">
                Morse → Text
              </option>
            </select>

            <ChevronDown className="pointer-events-none absolute right-3 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

          </div>

        </section>

        {/* =================================================
            HISTORY CONTENT
        ================================================= */}

        <section className="rounded-2xl border border-white/10 bg-white/[0.03]">

          {/* =================================================
              LOADING STATE
          ================================================= */}

          {loading ? (

            <div className="flex min-h-[420px] flex-col items-center justify-center px-6 py-16 text-center">

              <div className="mb-5 h-10 w-10 animate-spin rounded-full border-2 border-white/10 border-t-cyan-400" />

              <h2 className="text-xl font-semibold text-slate-300">
                Loading history...
              </h2>

              <p className="mt-3 text-sm text-slate-500">
                Fetching your translations from the C backend.
              </p>

            </div>

          ) : error ? (

            /* =================================================
               ERROR STATE
            ================================================= */

            <div className="flex min-h-[420px] flex-col items-center justify-center px-6 py-16 text-center">

              <div className="mb-5 flex h-16 w-16 items-center justify-center rounded-2xl bg-red-400/10">
                <HistoryIcon className="h-7 w-7 text-red-300" />
              </div>

              <h2 className="text-xl font-semibold text-slate-300">
                Unable to load history
              </h2>

              <p className="mt-3 max-w-md text-sm leading-6 text-slate-500">
                {error}
              </p>

              <Link
                to="/login"
                className="mt-6 rounded-xl bg-cyan-400 px-5 py-3 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300"
              >
                Go to Login
              </Link>

            </div>

          ) : filteredHistory.length === 0 ? (

            /* =================================================
               EMPTY STATE
            ================================================= */

            <div className="flex min-h-[420px] flex-col items-center justify-center px-6 py-16 text-center">

              <div className="mb-5 flex h-16 w-16 items-center justify-center rounded-2xl bg-white/5">
                <HistoryIcon className="h-7 w-7 text-slate-600" />
              </div>

              <h2 className="text-xl font-semibold text-slate-300">
                No translation history
              </h2>

              <p className="mt-3 max-w-md text-sm leading-6 text-slate-500">
                {search
                  ? 'No translations match your search.'
                  : 'Your completed translations will appear here automatically.'}
              </p>

              {!search && (
                <Link
                  to="/translator"
                  className="mt-6 inline-flex items-center gap-2 rounded-xl bg-cyan-400 px-5 py-3 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300"
                >
                  Start Translating
                </Link>
              )}

            </div>

          ) : (

            /* =================================================
               HISTORY LIST
            ================================================= */

            <div className="divide-y divide-white/10">

              {filteredHistory.map((item) => (

                <article
                  key={item.id}
                  className={compactMode ? 'p-4' : 'p-6'}
                >

                  <div className="flex flex-col gap-5 lg:flex-row lg:items-start lg:justify-between">

                    <div className="min-w-0 flex-1">

                      {/* =================================================
                          HISTORY META
                      ================================================= */}

                      <div className="mb-4 flex flex-wrap items-center gap-3">

                        <span className="rounded-full border border-cyan-400/20 bg-cyan-400/10 px-3 py-1 text-xs font-medium text-cyan-300">

                          {item.type === 'TEXT_TO_MORSE'
                            ? 'Text → Morse'
                            : 'Morse → Text'}

                        </span>

                        <span className="flex items-center gap-1.5 text-xs text-slate-600">

                          <CalendarDays className="h-3.5 w-3.5" />

                          {item.date}

                        </span>

                      </div>

                      {/* =================================================
                          INPUT + OUTPUT
                      ================================================= */}

                      <div className="grid gap-4 md:grid-cols-2">

                        {/* Input */}

                        <div>

                          <p className="mb-2 text-xs uppercase tracking-wider text-slate-600">
                            Input
                          </p>

                          <div className="rounded-xl border border-white/10 bg-black/20 p-4 font-mono text-sm text-slate-300">
                            {item.input}
                          </div>

                        </div>

                        {/* Output */}

                        <div>

                          <p className="mb-2 text-xs uppercase tracking-wider text-slate-600">
                            Output
                          </p>

                          <div className="rounded-xl border border-cyan-400/10 bg-cyan-400/[0.03] p-4 font-mono text-sm text-cyan-200">
                            {item.output}
                          </div>

                        </div>

                      </div>

                    </div>

                    {/* =================================================
                        ACTION BUTTONS
                    ================================================= */}

                    <div className="flex gap-2">

                      {/* Copy */}

                      <button
                        type="button"
                        onClick={() =>
                          copyTranslation(item)
                        }
                        className="rounded-lg border border-white/10 p-2.5 text-slate-500 transition hover:bg-white/5 hover:text-white"
                        title="Copy output"
                      >

                        {copiedId === item.id ? (
                          <Check className="h-4 w-4 text-green-400" />
                        ) : (
                          <Copy className="h-4 w-4" />
                        )}

                      </button>

                      {/* Delete */}

                      <button
                        type="button"
                        onClick={() => deleteTranslation(item.id)}
                        className="rounded-lg border border-white/10 p-2.5 text-slate-500 transition hover:bg-red-400/5 hover:text-red-300"
                        title="Delete translation"
                      >
                        <Trash2 className="h-4 w-4" />
                      </button>

                    </div>

                  </div>

                </article>

              ))}

            </div>

          )}

        </section>

      </main>

    </div>
  )
}

export default History