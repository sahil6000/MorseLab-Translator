const API_BASE_URL = 'http://localhost:8080'

type PreferencesResponse = {
  success: boolean
  preferences?: {
    activity_notifications_enabled: number
  }
}

export async function requestNotificationPermission(): Promise<boolean> {
  if (!('Notification' in window)) {
    return false
  }

  if (Notification.permission === 'granted') {
    return true
  }

  if (Notification.permission === 'denied') {
    return false
  }

  return (await Notification.requestPermission()) === 'granted'
}

export function showActivityNotification(
  title: string,
  body: string,
): void {
  if (!('Notification' in window) || Notification.permission !== 'granted') {
    return
  }

  new Notification(title, { body })
}

export async function notifyActivityIfEnabled(
  token: string,
  title: string,
  body: string,
): Promise<void> {
  if (!token || !('Notification' in window) || Notification.permission !== 'granted') {
    return
  }

  try {
    const response = await fetch(`${API_BASE_URL}/api/preferences`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ token }),
    })

    const data = (await response.json()) as PreferencesResponse
    if (
      response.ok &&
      data.success &&
      data.preferences?.activity_notifications_enabled === 1
    ) {
      showActivityNotification(title, body)
    }
  } catch {
    // Notifications should not interrupt the activity that just completed.
  }
}
