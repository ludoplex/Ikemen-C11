package main

import (
	"time"
)

// NotificationType defines different types of notifications
type NotificationType int

const (
	NotifyInfo NotificationType = iota
	NotifySuccess
	NotifyWarning
	NotifyError
)

// Notification represents a user-facing notification message
type Notification struct {
	Message     string
	Type        NotificationType
	Duration    time.Duration
	CreatedAt   time.Time
	ExpiresAt   time.Time
	DisplayPosY float32 // Y position for this notification in the stack
}

// NotificationManager manages the display of user notifications
type NotificationManager struct {
	notifications []*Notification
	maxVisible    int
	spacing       float32 // Vertical spacing between notifications
	fadeInTime    time.Duration
	fadeOutTime   time.Duration
}

// NewNotificationManager creates a new notification manager
func NewNotificationManager() *NotificationManager {
	return &NotificationManager{
		notifications: make([]*Notification, 0),
		maxVisible:    5,
		spacing:       30,
		fadeInTime:    time.Millisecond * 300,
		fadeOutTime:   time.Millisecond * 300,
	}
}

// AddNotification adds a new notification to the display queue
func (nm *NotificationManager) AddNotification(message string, notifType NotificationType, duration time.Duration) {
	now := time.Now()
	notification := &Notification{
		Message:   message,
		Type:      notifType,
		Duration:  duration,
		CreatedAt: now,
		ExpiresAt: now.Add(duration),
	}
	nm.notifications = append(nm.notifications, notification)
}

// Update removes expired notifications and updates positions
func (nm *NotificationManager) Update() {
	now := time.Now()

	// Remove expired notifications
	activeNotifications := make([]*Notification, 0)
	for _, notif := range nm.notifications {
		if now.Before(notif.ExpiresAt) {
			activeNotifications = append(activeNotifications, notif)
		}
	}
	nm.notifications = activeNotifications

	// Keep only the most recent notifications if we exceed max visible
	if len(nm.notifications) > nm.maxVisible {
		nm.notifications = nm.notifications[len(nm.notifications)-nm.maxVisible:]
	}
}

// GetVisibleNotifications returns notifications that should be displayed
func (nm *NotificationManager) GetVisibleNotifications() []*Notification {
	return nm.notifications
}

// GetOpacity calculates the opacity for a notification based on its age
func (nm *NotificationManager) GetOpacity(notif *Notification) float32 {
	now := time.Now()
	age := now.Sub(notif.CreatedAt)
	timeUntilExpiry := notif.ExpiresAt.Sub(now)

	// Fade in
	if age < nm.fadeInTime {
		return float32(age) / float32(nm.fadeInTime)
	}

	// Fade out
	if timeUntilExpiry < nm.fadeOutTime {
		return float32(timeUntilExpiry) / float32(nm.fadeOutTime)
	}

	// Fully visible
	return 1.0
}

// Clear removes all notifications
func (nm *NotificationManager) Clear() {
	nm.notifications = make([]*Notification, 0)
}

// Configure updates notification manager settings from config
func (nm *NotificationManager) Configure(maxVisible int, spacing float32) {
	if maxVisible > 0 {
		nm.maxVisible = maxVisible
	}
	if spacing > 0 {
		nm.spacing = spacing
	}
}
