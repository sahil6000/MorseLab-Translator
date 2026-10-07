import {
  ArrowLeft,
  Bookmark,
  CalendarDays,
  Check,
  Copy,
  Search,
  Trash2,
  Pencil,
} from 'lucide-react'
import { useEffect, useState } from 'react'
import { Link, useNavigate } from 'react-router-dom'

type SavedItem = {
  id: number
  title: string
  input_type: 'TEXT_TO_MORSE' | 'MORSE_TO_TEXT'
  input: string
  output: string
  created_at: string
  updated_at: string
}

function formatSavedDate(dateString: string): string {
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

function Saved() {
    const navigate = useNavigate()
  const [search, setSearch] = useState('')
  const [copiedId, setCopiedId] = useState<number | null>(null)
  const [savedItems, setSavedItems] = useState<SavedItem[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')

    useEffect(() => {
    const loadSavedTranslations = async () => {
      const token =
        localStorage.getItem('morselab_token') ||
        sessionStorage.getItem('morselab_token')

      if (!token) {
        setError('Your session has expired. Please login again.')
        setLoading(false)
        return
      }

      try {
        setError('')

        const response = await fetch(
          'http://localhost:8080/api/saved/list',
          {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify({ token }),
          },
        )

        const data = await response.json()

        if (!response.ok || !data.success) {
          throw new Error(
            data.error || 'Failed to load saved translations.',
          )
        }

        setSavedItems(data.saved || [])
      } catch (requestError) {
        setError(
          requestError instanceof Error
            ? requestError.message
            : 'Failed to load saved translations.',
        )
      } finally {
        setLoading(false)
      }
    }

    loadSavedTranslations()
  }, [])

  const filteredItems = savedItems.filter(
    (item) =>
      item.input.toLowerCase().includes(search.toLowerCase()) ||
      item.output.toLowerCase().includes(search.toLowerCase()),
  )

  const copyTranslation = async (item: SavedItem) => {
    await navigator.clipboard.writeText(item.output)
    setCopiedId(item.id)

    setTimeout(() => {
      setCopiedId(null)
    }, 1500)
  }

    const deleteTranslation = async (savedId: number) => {
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
        'http://localhost:8080/api/saved',
        {
          method: 'DELETE',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            token,
            saved_id: savedId,
          }),
        },
      )

      const data = await response.json()

      if (!response.ok || !data.success) {
        throw new Error(
          data.error || 'Failed to delete saved translation.',
        )
      }

      setSavedItems((currentItems) =>
        currentItems.filter((item) => item.id !== savedId),
      )
    } catch (requestError) {
      setError(
        requestError instanceof Error
          ? requestError.message
          : 'Failed to delete saved translation.',
      )
    }
  }

  return (
    <div className="min-h-screen bg-[#050816] text-white">
      <header className="border-b border-white/10 bg-[#070b1a]/90 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-6 py-5">
          <Link to="/dashboard" className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-violet-400/30 bg-violet-400/10">
              <Bookmark className="h-5 w-5 text-violet-300" />
            </div>

            <div>
              <div className="font-bold tracking-widest">MORSELAB</div>
              <div className="text-xs text-slate-500">Saved</div>
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
            Saved Translations
          </h1>

          <p className="mt-3 max-w-2xl text-slate-400">
            Keep important Morse translations available for quick access and
            reuse.
          </p>
        </div>

        <section className="mb-6 rounded-2xl border border-white/10 bg-white/[0.03] p-4">
          <div className="relative">
            <Search className="absolute left-3.5 top-1/2 h-4 w-4 -translate-y-1/2 text-slate-600" />

            <input
              type="text"
              value={search}
              onChange={(event) => setSearch(event.target.value)}
              placeholder="Search saved translations..."
              className="w-full rounded-xl border border-white/10 bg-black/20 py-3 pl-10 pr-4 text-sm text-white outline-none placeholder:text-slate-700 focus:border-violet-400/40"
            />
          </div>
        </section>

        <section className="rounded-2xl border border-white/10 bg-white/[0.03]">
  {loading ? (
    <div className="flex min-h-[420px] items-center justify-center px-6 py-16 text-center">
      <div>
        <div className="mx-auto mb-5 h-10 w-10 animate-spin rounded-full border-2 border-white/10 border-t-cyan-400" />
        <p className="text-sm text-slate-500">
          Loading saved translations...
        </p>
      </div>
    </div>
  ) : error ? (
    <div className="flex min-h-[420px] flex-col items-center justify-center px-6 py-16 text-center">
      <div className="mb-5 flex h-16 w-16 items-center justify-center rounded-2xl bg-red-400/10">
        <Bookmark className="h-7 w-7 text-red-400/60" />
      </div>

      <h2 className="text-xl font-semibold text-slate-300">
        Unable to load saved translations
      </h2>

      <p className="mt-3 max-w-md text-sm leading-6 text-red-300/70">
        {error}
      </p>
    </div>
  ) : filteredItems.length === 0 ? (
            <div className="flex min-h-[420px] flex-col items-center justify-center px-6 py-16 text-center">
              <div className="mb-5 flex h-16 w-16 items-center justify-center rounded-2xl bg-violet-400/10">
                <Bookmark className="h-7 w-7 text-violet-400/50" />
              </div>

              <h2 className="text-xl font-semibold text-slate-300">
                No saved translations
              </h2>

              <p className="mt-3 max-w-md text-sm leading-6 text-slate-500">
                {search
                  ? 'No saved translations match your search.'
                  : 'Translations you save from the translator will appear here.'}
              </p>

              {!search && (
                <Link
                  to="/translator"
                  className="mt-6 inline-flex items-center gap-2 rounded-xl bg-cyan-400 px-5 py-3 text-sm font-semibold text-slate-950 transition hover:bg-cyan-300"
                >
                  Open Translator
                </Link>
              )}
            </div>
          ) : (
            <div className="divide-y divide-white/10">
              {filteredItems.map((item) => (
                <article key={item.id} className="p-6">
                  <div className="flex flex-col gap-5 lg:flex-row lg:items-start lg:justify-between">
                    <div className="min-w-0 flex-1">
                      <div className="mb-4 flex flex-wrap items-center gap-3">
                        <span className="rounded-full border border-violet-400/20 bg-violet-400/10 px-3 py-1 text-xs font-medium text-violet-300">
                          {item.input_type === 'TEXT_TO_MORSE'
                            ? 'Text → Morse'
                            : 'Morse → Text'}
                        </span>

                        <span className="flex items-center gap-1.5 text-xs text-slate-600">
                          <CalendarDays className="h-3.5 w-3.5" />
                          {formatSavedDate(item.created_at)}
                        </span>
                      </div>

                      <div className="grid gap-4 md:grid-cols-2">
                        <div>
                          <p className="mb-2 text-xs uppercase tracking-wider text-slate-600">
                            Input
                          </p>

                          <div className="rounded-xl border border-white/10 bg-black/20 p-4 font-mono text-sm text-slate-300">
                            {item.input}
                          </div>
                        </div>

                        <div>
                          <p className="mb-2 text-xs uppercase tracking-wider text-slate-600">
                            Output
                          </p>

                          <div className="rounded-xl border border-violet-400/10 bg-violet-400/[0.03] p-4 font-mono text-sm text-violet-200">
                            {item.output}
                          </div>
                        </div>
                      </div>
                    </div>

                    <div className="flex gap-2">
                      <button
                        type="button"
                        onClick={() => copyTranslation(item)}
                        className="rounded-lg border border-white/10 p-2.5 text-slate-500 transition hover:bg-white/5 hover:text-white"
                        title="Copy output"
                      >
                        {copiedId === item.id ? (
                          <Check className="h-4 w-4 text-green-400" />
                        ) : (
                          <Copy className="h-4 w-4" />
                        )}
                      </button>

<button
  type="button"
  onClick={() =>
    navigate('/translator', {
      state: {
        editSavedId: item.id,
        title: item.title,
        input: item.input,
        output: item.output,
        input_type: item.input_type,
      },
    })
  }
  className="rounded-lg border border-white/10 p-2.5 text-slate-500 transition hover:bg-white/5 hover:text-white"
  title="Edit saved translation"
>
  <Pencil className="h-4 w-4" />
</button>

                      <button
                        type="button"
                        onClick={() => deleteTranslation(item.id)}
                        className="rounded-lg border border-white/10 p-2.5 text-slate-500 transition hover:bg-red-400/5 hover:text-red-300"
                        title="Delete saved translation"
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

export default Saved