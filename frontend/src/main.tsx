import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import { BrowserRouter } from 'react-router-dom'

import './index.css'
import App from './App.tsx'

const routerBasename = import.meta.env.BASE_URL.replace(/\/$/, '') || '/'

document.documentElement.classList.toggle(
  'compact-mode',
  localStorage.getItem('morselab_compact_mode') === 'true',
)

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <BrowserRouter basename={routerBasename}>
      <App />
    </BrowserRouter>
  </StrictMode>,
)
