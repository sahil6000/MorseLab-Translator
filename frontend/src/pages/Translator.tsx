import {
  ArrowLeftRight,
  Check,
  Clipboard,
  Copy,
  Languages,
  Loader2,
  RotateCcw,
  Save,
  Sparkles,
  Trash2,
} from 'lucide-react'
import { useEffect, useState } from 'react'
import { Link, useLocation } from 'react-router-dom'
import { notifyActivityIfEnabled } from '../utils/notifications'

type TranslationMode = 'text-to-morse' | 'morse-to-text'

function Translator() {
    const location = useLocation()
  const [mode, setMode] = useState<TranslationMode>('text-to-morse')
  const [input, setInput] = useState('')
  const [output, setOutput] = useState('')
  const [loading, setLoading] = useState(false)
  const [copied, setCopied] = useState(false)
  const [saved, setSaved] = useState(false)
  const [error, setError] = useState('')

  const [editingSavedId, setEditingSavedId] = useState<number | null>(null)

  useEffect(() => {
    const editData = location.state as
      | {
          editSavedId?: number
          input?: string
          output?: string
          input_type?: 'TEXT_TO_MORSE' | 'MORSE_TO_TEXT'
        }
      | null

    if (editData?.editSavedId) {
      setEditingSavedId(editData.editSavedId)
      setInput(editData.input || '')
      setOutput(editData.output || '')
      setMode(
        editData.input_type === 'MORSE_TO_TEXT'
          ? 'morse-to-text'
          : 'text-to-morse',
      )
      setSaved(false)
    }
  }, [location.state])

  const translate = async () => {
    if (!input.trim()) {
      setError('Please enter something to translate.')
      setOutput('')
      return
    }

    setError('')
    setLoading(true)
    setSaved(false)

    try {
      const endpoint =
        mode === 'text-to-morse'
          ? '/api/translate/text-to-morse'
          : '/api/translate/morse-to-text'

      const token =
  localStorage.getItem('morselab_token') ||
  sessionStorage.getItem('morselab_token')

if (!token) {
  setError('Your session has expired. Please login again.')
  setLoading(false)
  return
}

const response = await fetch(`http://localhost:8080${endpoint}`, {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
  },
  body: JSON.stringify({
    token,
    text: input,
  }),
})

      if (!response.ok) {
        throw new Error('Translation request failed.')
      }

      const data = await response.json()

      if (!data.success) {
        throw new Error(data.message || 'Translation failed.')
      }

      setOutput(data.output || '')
      void notifyActivityIfEnabled(
        token,
        'MorseLab Translation',
        'A translation was completed.',
      )
    } catch {
      setError(
        'Unable to connect to the C backend. Make sure the MorseLab backend server is running.'
      )
    } finally {
      setLoading(false)
    }
  }

  const swapMode = () => {
    const newMode =
      mode === 'text-to-morse' ? 'morse-to-text' : 'text-to-morse'

    setMode(newMode)
    setInput(output)
    setOutput(input)
    setError('')
    setCopied(false)
    setSaved(false)
  }

  const clearAll = () => {
    setInput('')
    setOutput('')
    setError('')
    setCopied(false)
    setSaved(false)
  }

  const copyOutput = async () => {
    if (!output) return

    try {
      await navigator.clipboard.writeText(output)
      setCopied(true)

      setTimeout(() => {
        setCopied(false)
      }, 1800)
    } catch {
      setError('Unable to copy the translation.')
    }
  }

    const saveTranslation = async () => {
    if (!output) return

    const token =
      localStorage.getItem('morselab_token') ||
      sessionStorage.getItem('morselab_token')

    if (!token) {
      setError('Your session has expired. Please login again.')
      return
    }

    try {
      setError('')

      const title =
        mode === 'text-to-morse'
          ? 'Text to Morse Translation'
          : 'Morse to Text Translation'

      const inputType =
        mode === 'text-to-morse'
          ? 'TEXT_TO_MORSE'
          : 'MORSE_TO_TEXT'

      const endpoint = editingSavedId
        ? 'http://localhost:8080/api/saved/update'
        : 'http://localhost:8080/api/saved'

      const method = editingSavedId ? 'POST' : 'POST'

      const body = editingSavedId
        ? {
            token,
            saved_id: editingSavedId,
            title,
            input,
            output,
            input_type: inputType,
          }
        : {
            token,
            title,
            input,
            output,
            input_type: inputType,
          }

      const response = await fetch(endpoint, {
        method,
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(body),
      })

      const data = await response.json()

      if (!response.ok || !data.success) {
        throw new Error(
          data.error ||
            (editingSavedId
              ? 'Failed to update saved translation.'
              : 'Failed to save translation.'),
        )
      }

      setSaved(true)
      void notifyActivityIfEnabled(
        token,
        'MorseLab Saved Translations',
        editingSavedId
          ? 'A saved translation was updated.'
          : 'A translation was saved.',
      )

      setTimeout(() => {
        setSaved(false)
      }, 1800)
    } catch (requestError) {
      setError(
        requestError instanceof Error
          ? requestError.message
          : editingSavedId
            ? 'Failed to update saved translation.'
            : 'Failed to save translation.',
      )
    }
  }

  return (
    <div className="compact-translator min-h-screen bg-[#050816] text-white">
      {/* Header */}
      <header className="border-b border-white/10 bg-[#070b1a]/90 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-6 py-5">
          <Link to="/dashboard" className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <Languages className="h-5 w-5 text-cyan-300" />
            </div>

            <div>
              <div className="font-bold tracking-widest">MORSELAB</div>
              <div className="text-xs text-slate-500">Translator</div>
            </div>
          </Link>

          <Link
            to="/dashboard"
            className="rounded-lg border border-white/10 px-4 py-2 text-sm text-slate-300 transition hover:bg-white/5"
          >
            Dashboard
          </Link>
        </div>
      </header>

      {/* Main */}
      <main className="mx-auto max-w-6xl px-6 py-10">
        {/* Heading */}
        <section className="compact-translator-heading mb-8">
          <div className="mb-3 flex items-center gap-2 text-cyan-300">
            <Sparkles className="h-4 w-4" />
            <span className="text-sm font-medium">
              C-powered translation engine
            </span>
          </div>

          <h1 className="text-4xl font-bold tracking-tight md:text-5xl">
            Morse Translator
          </h1>

          <p className="mt-3 max-w-2xl text-slate-400">
            Convert plain text into Morse code or decode Morse code into
            readable text using the MorseLab backend.
          </p>
        </section>

        {/* Translator Card */}
        <section className="compact-translation-card rounded-3xl border border-white/10 bg-white/[0.03] p-5 shadow-2xl shadow-black/20 md:p-7">
          {/* Mode Selector */}
          <div className="mb-7 flex flex-col gap-4 md:flex-row md:items-center md:justify-between">
            <div className="inline-flex rounded-xl border border-white/10 bg-black/20 p-1">
              <button
                type="button"
                onClick={() => {
                  setMode('text-to-morse')
                  setOutput('')
                  setError('')
                }}
                className={`rounded-lg px-5 py-2.5 text-sm font-medium transition ${
                  mode === 'text-to-morse'
                    ? 'bg-cyan-400 text-slate-950'
                    : 'text-slate-400 hover:text-white'
                }`}
              >
                Text → Morse
              </button>

              <button
                type="button"
                onClick={() => {
                  setMode('morse-to-text')
                  setOutput('')
                  setError('')
                }}
                className={`rounded-lg px-5 py-2.5 text-sm font-medium transition ${
                  mode === 'morse-to-text'
                    ? 'bg-cyan-400 text-slate-950'
                    : 'text-slate-400 hover:text-white'
                }`}
              >
                Morse → Text
              </button>
            </div>

            <button
              type="button"
              onClick={swapMode}
              className="inline-flex items-center justify-center gap-2 rounded-lg border border-white/10 px-4 py-2.5 text-sm text-slate-300 transition hover:bg-white/5"
            >
              <ArrowLeftRight className="h-4 w-4" />
              Swap
            </button>
          </div>

          {/* Translation Areas */}
          <div className="compact-translation-grid grid gap-5 lg:grid-cols-2">
            {/* Input */}
            <div>
              <div className="mb-3 flex items-center justify-between">
                <label className="text-sm font-medium text-slate-300">
                  {mode === 'text-to-morse' ? 'Text Input' : 'Morse Input'}
                </label>

                <span className="text-xs text-slate-600">
                  {input.length} characters
                </span>
              </div>

              <textarea
                value={input}
                onChange={(event) => {
                  setInput(event.target.value)
                  setError('')
                }}
                placeholder={
                  mode === 'text-to-morse'
                    ? 'Type your message here...'
                    : '.... . .-.. .-.. --- / .-- --- .-. .-.. -..'
                }
                className="compact-translation-input min-h-[300px] w-full resize-none rounded-2xl border border-white/10 bg-[#030610] p-5 font-mono text-sm text-white outline-none transition placeholder:text-slate-700 focus:border-cyan-400/50 focus:ring-2 focus:ring-cyan-400/10"
              />
            </div>

            {/* Output */}
            <div>
              <div className="mb-3 flex items-center justify-between">
                <label className="text-sm font-medium text-slate-300">
                  {mode === 'text-to-morse'
                    ? 'Morse Output'
                    : 'Text Output'}
                </label>

                <span className="text-xs text-slate-600">
                  {output.length} characters
                </span>
              </div>

              <div className="compact-translation-output relative min-h-[300px] rounded-2xl border border-white/10 bg-[#030610]">
                {output ? (
                  <div className="compact-translation-content h-full min-h-[300px] whitespace-pre-wrap break-words p-5 font-mono text-sm text-cyan-200">
                    {output}
                  </div>
                ) : (
                  <div className="flex min-h-[300px] items-center justify-center p-8 text-center">
                    <div>
                      <div className="mx-auto mb-4 flex h-12 w-12 items-center justify-center rounded-xl bg-white/5">
                        <Languages className="h-5 w-5 text-slate-600" />
                      </div>

                      <p className="text-sm text-slate-600">
                        Your translation will appear here.
                      </p>
                    </div>
                  </div>
                )}
              </div>
            </div>
          </div>

          {/* Error */}
          {error && (
            <div className="mt-5 rounded-xl border border-red-400/20 bg-red-400/5 px-4 py-3 text-sm text-red-300">
              {error}
            </div>
          )}

          {/* Actions */}
          <div className="mt-6 flex flex-col gap-3 sm:flex-row sm:flex-wrap">
            <button
              type="button"
              onClick={translate}
              disabled={loading}
              className="inline-flex flex-1 items-center justify-center gap-2 rounded-xl bg-cyan-400 px-6 py-3 font-semibold text-slate-950 transition hover:bg-cyan-300 disabled:cursor-not-allowed disabled:opacity-60 sm:flex-none"
            >
              {loading ? (
                <>
                  <Loader2 className="h-4 w-4 animate-spin" />
                  Translating...
                </>
              ) : (
                <>
                  <Languages className="h-4 w-4" />
                  Translate
                </>
              )}
            </button>

            <button
              type="button"
              onClick={copyOutput}
              disabled={!output}
              className="inline-flex items-center justify-center gap-2 rounded-xl border border-white/10 px-5 py-3 text-sm text-slate-300 transition hover:bg-white/5 disabled:cursor-not-allowed disabled:opacity-40"
            >
              {copied ? (
                <>
                  <Check className="h-4 w-4 text-green-400" />
                  Copied
                </>
              ) : (
                <>
                  <Copy className="h-4 w-4" />
                  Copy
                </>
              )}
            </button>

            <button
              type="button"
              onClick={saveTranslation}
              disabled={!output}
              className="inline-flex items-center justify-center gap-2 rounded-xl border border-white/10 px-5 py-3 text-sm text-slate-300 transition hover:bg-white/5 disabled:cursor-not-allowed disabled:opacity-40"
            >
              {saved ? (
                <>
                  <Check className="h-4 w-4 text-green-400" />
                  Saved
                </>
              ) : (
                <>
                  <Save className="h-4 w-4" />
                  Save
                </>
              )}
            </button>

            <button
              type="button"
              onClick={clearAll}
              className="inline-flex items-center justify-center gap-2 rounded-xl border border-white/10 px-5 py-3 text-sm text-slate-400 transition hover:bg-white/5 hover:text-white"
            >
              <Trash2 className="h-4 w-4" />
              Clear
            </button>
          </div>
        </section>

        {/* Information */}
        <section className="compact-translator-info mt-6 grid gap-4 md:grid-cols-3">
          <div className="compact-translator-info-card rounded-2xl border border-white/10 bg-white/[0.02] p-5">
            <div className="mb-3 text-cyan-300">
              <Languages className="h-5 w-5" />
            </div>
            <h3 className="font-medium">Bidirectional</h3>
            <p className="mt-2 text-sm text-slate-500">
              Translate both text to Morse and Morse to text.
            </p>
          </div>

          <div className="compact-translator-info-card rounded-2xl border border-white/10 bg-white/[0.02] p-5">
            <div className="mb-3 text-cyan-300">
              <Clipboard className="h-5 w-5" />
            </div>
            <h3 className="font-medium">Quick Copy</h3>
            <p className="mt-2 text-sm text-slate-500">
              Copy translated results directly to your clipboard.
            </p>
          </div>

          <div className="compact-translator-info-card rounded-2xl border border-white/10 bg-white/[0.02] p-5">
            <div className="mb-3 text-cyan-300">
              <RotateCcw className="h-5 w-5" />
            </div>
            <h3 className="font-medium">Fast Processing</h3>
            <p className="mt-2 text-sm text-slate-500">
              Requests are processed through the C backend translation engine.
            </p>
          </div>
        </section>
      </main>
    </div>
  )
}

export default Translator
