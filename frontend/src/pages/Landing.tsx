import { ArrowRight, Binary, Clock3, Database, ShieldCheck } from 'lucide-react'
import { Link } from 'react-router-dom'

function Landing() {
  return (
    <div className="min-h-screen bg-[#050816] text-white">
      {/* Navigation */}
      <header className="border-b border-white/10 bg-[#050816]/80 backdrop-blur-xl">
        <div className="mx-auto flex h-20 max-w-7xl items-center justify-between px-6 lg:px-8">
          <Link to="/" className="flex items-center gap-3">
            <div className="flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/30 bg-cyan-400/10">
              <Binary className="h-5 w-5 text-cyan-300" />
            </div>

            <div>
              <p className="text-lg font-bold tracking-wide">MORSELAB</p>
              <p className="text-[10px] uppercase tracking-[0.25em] text-slate-500">
                Translation Engine
              </p>
            </div>
          </Link>

          <nav className="hidden items-center gap-8 text-sm text-slate-400 md:flex">
            <a href="#features" className="transition hover:text-white">
              Features
            </a>
            <a href="#technology" className="transition hover:text-white">
              Technology
            </a>
          </nav>

          <div className="flex items-center gap-3">
            <Link
              to="/login"
              className="hidden rounded-xl px-4 py-2.5 text-sm font-medium text-slate-300 transition hover:bg-white/5 hover:text-white sm:block"
            >
              Login
            </Link>

            <Link
              to="/signup"
              className="rounded-xl border border-cyan-400/30 bg-cyan-400/10 px-4 py-2.5 text-sm font-semibold text-cyan-300 transition hover:bg-cyan-400/20"
            >
              Get Started
            </Link>
          </div>
        </div>
      </header>

      {/* Hero */}
      <main>
        <section className="relative overflow-hidden">
          <div className="absolute left-1/2 top-20 h-96 w-96 -translate-x-1/2 rounded-full bg-cyan-400/10 blur-3xl" />

          <div className="relative mx-auto max-w-7xl px-6 pb-24 pt-24 text-center lg:px-8 lg:pb-32 lg:pt-32">
            <div className="mx-auto mb-7 inline-flex items-center gap-2 rounded-full border border-cyan-400/20 bg-cyan-400/5 px-4 py-2 text-sm text-cyan-300">
              <span className="h-1.5 w-1.5 rounded-full bg-cyan-300" />
              C-POWERED TRANSLATION ENGINE
            </div>

            <h1 className="mx-auto max-w-5xl text-5xl font-bold tracking-tight sm:text-6xl lg:text-8xl">
              Decode the signal.
              <span className="block bg-gradient-to-r from-cyan-300 via-sky-400 to-indigo-400 bg-clip-text text-transparent">
                Understand the message.
              </span>
            </h1>

            <p className="mx-auto mt-7 max-w-2xl text-base leading-7 text-slate-400 sm:text-lg">
              MorseLab is a professional Morse code translation platform
              built around a pure C backend, real data structures, and a
              persistent SQLite database.
            </p>

            <div className="mt-10 flex flex-col items-center justify-center gap-4 sm:flex-row">
              <Link
                to="/translator"
                className="group inline-flex items-center gap-2 rounded-xl bg-cyan-400 px-6 py-3.5 font-semibold text-slate-950 shadow-lg shadow-cyan-400/10 transition hover:bg-cyan-300"
              >
                Open Translator
                <ArrowRight className="h-4 w-4 transition group-hover:translate-x-1" />
              </Link>

              <Link
                to="/signup"
                className="rounded-xl border border-white/10 bg-white/[0.03] px-6 py-3.5 font-semibold text-slate-200 transition hover:border-white/20 hover:bg-white/[0.06]"
              >
                Create Account
              </Link>
            </div>

            <div className="mx-auto mt-16 max-w-4xl rounded-2xl border border-white/10 bg-white/[0.02] p-2 shadow-2xl shadow-cyan-950/20">
              <div className="rounded-xl border border-white/5 bg-[#090d1c] p-6 text-left">
                <div className="mb-6 flex items-center gap-2">
                  <span className="h-2.5 w-2.5 rounded-full bg-red-400/70" />
                  <span className="h-2.5 w-2.5 rounded-full bg-yellow-400/70" />
                  <span className="h-2.5 w-2.5 rounded-full bg-green-400/70" />
                  <span className="ml-3 text-xs text-slate-600">
                    morselab / translator
                  </span>
                </div>

                <div className="grid gap-6 md:grid-cols-2">
                  <div>
                    <p className="mb-3 text-xs font-semibold uppercase tracking-widest text-slate-500">
                      Input
                    </p>
                    <div className="rounded-xl border border-white/10 bg-black/20 p-5 font-mono text-lg text-slate-200">
                      HELLO WORLD
                    </div>
                  </div>

                  <div>
                    <p className="mb-3 text-xs font-semibold uppercase tracking-widest text-slate-500">
                      Morse Output
                    </p>
                    <div className="rounded-xl border border-cyan-400/10 bg-cyan-400/[0.03] p-5 font-mono text-lg text-cyan-300">
                      .... . .-.. .-.. --- / .-- --- .-. .-.. -..
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </section>

        {/* Features */}
        <section id="features" className="border-y border-white/10 bg-white/[0.015]">
          <div className="mx-auto max-w-7xl px-6 py-24 lg:px-8">
            <div className="max-w-2xl">
              <p className="text-sm font-semibold uppercase tracking-[0.2em] text-cyan-300">
                Built for the project
              </p>

              <h2 className="mt-4 text-3xl font-bold tracking-tight sm:text-4xl">
                More than a simple translator.
              </h2>

              <p className="mt-4 text-slate-400">
                Every major layer is designed around the project's actual
                architecture and data-structure requirements.
              </p>
            </div>

            <div className="mt-12 grid gap-5 md:grid-cols-3">
              <FeatureCard
                icon={<Binary className="h-5 w-5" />}
                title="Real DSA"
                description="Morse binary tree, hash table, linked list, stack, and queue are used in the backend."
              />

              <FeatureCard
                icon={<Database className="h-5 w-5" />}
                title="Persistent Data"
                description="SQLite stores users, translation history, and saved translations."
              />

              <FeatureCard
                icon={<ShieldCheck className="h-5 w-5" />}
                title="Secure Architecture"
                description="Authentication, validation, authorization, and parameterized database operations."
              />
            </div>
          </div>
        </section>

        {/* Technology */}
        <section id="technology">
          <div className="mx-auto max-w-7xl px-6 py-24 lg:px-8">
            <div className="grid gap-12 lg:grid-cols-2 lg:items-center">
              <div>
                <p className="text-sm font-semibold uppercase tracking-[0.2em] text-cyan-300">
                  Technology
                </p>

                <h2 className="mt-4 text-3xl font-bold tracking-tight sm:text-4xl">
                  Three layers. One system.
                </h2>

                <p className="mt-5 max-w-xl leading-7 text-slate-400">
                  The React frontend communicates with the C HTTP backend,
                  while the backend manages the translation engine, data
                  structures, authentication, and SQLite persistence.
                </p>
              </div>

              <div className="grid gap-4">
                <TechRow label="Frontend" value="React + TypeScript + Vite + Tailwind" />
                <TechRow label="Backend" value="Pure C + libmicrohttpd + DSA" />
                <TechRow label="Database" value="SQLite + Prepared Statements" />
                <TechRow label="Translation" value="Hash Table + Morse Binary Tree" />
              </div>
            </div>
          </div>
        </section>

        {/* CTA */}
        <section className="border-t border-white/10">
          <div className="mx-auto max-w-7xl px-6 py-20 text-center lg:px-8">
            <Clock3 className="mx-auto h-7 w-7 text-cyan-300" />

            <h2 className="mt-5 text-3xl font-bold">
              Ready to translate?
            </h2>

            <p className="mx-auto mt-3 max-w-xl text-slate-400">
              Start working with MorseLab's translation engine.
            </p>

            <Link
              to="/translator"
              className="mt-7 inline-flex items-center gap-2 rounded-xl bg-cyan-400 px-6 py-3.5 font-semibold text-slate-950 transition hover:bg-cyan-300"
            >
              Launch Translator
              <ArrowRight className="h-4 w-4" />
            </Link>
          </div>
        </section>
      </main>

      {/* Footer */}
      <footer className="border-t border-white/10">
        <div className="mx-auto flex max-w-7xl flex-col gap-3 px-6 py-7 text-sm text-slate-500 sm:flex-row sm:items-center sm:justify-between lg:px-8">
          <p>© 2026 MorseLab. DSA in C project.</p>
          <p>React · C · SQLite</p>
        </div>
      </footer>
    </div>
  )
}

function FeatureCard({
  icon,
  title,
  description,
}: {
  icon: React.ReactNode
  title: string
  description: string
}) {
  return (
    <div className="rounded-2xl border border-white/10 bg-white/[0.02] p-6 transition hover:border-cyan-400/20 hover:bg-white/[0.035]">
      <div className="mb-5 flex h-10 w-10 items-center justify-center rounded-xl border border-cyan-400/20 bg-cyan-400/10 text-cyan-300">
        {icon}
      </div>

      <h3 className="text-lg font-semibold">{title}</h3>

      <p className="mt-2 text-sm leading-6 text-slate-400">{description}</p>
    </div>
  )
}

function TechRow({ label, value }: { label: string; value: string }) {
  return (
    <div className="flex flex-col gap-2 rounded-xl border border-white/10 bg-white/[0.02] p-5 sm:flex-row sm:items-center sm:justify-between">
      <span className="text-sm font-semibold text-slate-300">{label}</span>
      <span className="text-sm text-slate-500 sm:text-right">{value}</span>
    </div>
  )
}

export default Landing