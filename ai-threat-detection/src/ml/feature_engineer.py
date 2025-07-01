import time
from datetime import datetime
from typing import Dict, Any, List
import json

class SecurityFeatureEngineer:
    """Converts security events into ML-ready features"""
    
    def __init__(self):
        self.user_history = {}  # Track user behavior patterns
        self.ip_history = {}    # Track IP address patterns
        
    def extract_features(self, event: Dict[str, Any]) -> Dict[str, float]:
        """Extract ML features from a security event"""
        
        user_id = event.get('user_id', 'unknown')
        src_ip = event.get('src_ip', '0.0.0.0')
        event_type = event.get('event_type', 'unknown')
        timestamp = event.get('timestamp', time.time())
        
        # Update history
        self._update_user_history(user_id, event)
        self._update_ip_history(src_ip, event)
        
        # Extract features
        features = {}
        
        # Time-based features
        dt = datetime.fromtimestamp(timestamp)
        features['hour_of_day'] = dt.hour / 24.0  # Normalize to 0-1
        features['day_of_week'] = dt.weekday() / 6.0
        features['is_weekend'] = 1.0 if dt.weekday() >= 5 else 0.0
        features['is_night_time'] = 1.0 if dt.hour < 6 or dt.hour > 22 else 0.0
        features['is_business_hours'] = 1.0 if 9 <= dt.hour <= 17 and dt.weekday() < 5 else 0.0
        
        # User behavior features
        user_events = self.user_history.get(user_id, [])
        features['user_events_last_hour'] = len([e for e in user_events if e['timestamp'] > timestamp - 3600])
        features['user_unique_ips_today'] = len(set(e.get('src_ip') for e in user_events if e['timestamp'] > timestamp - 86400))
        features['user_failed_login_ratio'] = self._calculate_failure_rate(user_events)
        
        # IP behavior features  
        ip_events = self.ip_history.get(src_ip, [])
        features['ip_events_last_hour'] = len([e for e in ip_events if e['timestamp'] > timestamp - 3600])
        features['ip_unique_users'] = len(set(e.get('user_id') for e in ip_events))
        features['ip_is_external'] = 1.0 if not self._is_internal_ip(src_ip) else 0.0
        
        # Event type features
        features['is_login_event'] = 1.0 if 'login' in event_type else 0.0
        features['is_failed_event'] = 1.0 if 'failed' in event_type else 0.0
        features['is_admin_event'] = 1.0 if 'admin' in event_type else 0.0
        features['is_privilege_event'] = 1.0 if 'privilege' in event_type else 0.0
        
        # Risk indicators
        features['new_ip_for_user'] = 1.0 if self._is_new_ip_for_user(user_id, src_ip) else 0.0
        features['high_frequency_user'] = 1.0 if features['user_events_last_hour'] > 50 else 0.0
        features['suspicious_timing'] = 1.0 if features['is_night_time'] and features['user_events_last_hour'] > 5 else 0.0
        
        return features
    
    def _update_user_history(self, user_id: str, event: Dict[str, Any]):
        """Update user behavior history"""
        if user_id not in self.user_history:
            self.user_history[user_id] = []
        
        self.user_history[user_id].append(event)
        
        # Keep only last 24 hours of events
        cutoff = time.time() - 86400
        self.user_history[user_id] = [e for e in self.user_history[user_id] if e.get('timestamp', 0) > cutoff]
    
    def _update_ip_history(self, src_ip: str, event: Dict[str, Any]):
        """Update IP address history"""
        if src_ip not in self.ip_history:
            self.ip_history[src_ip] = []
        
        self.ip_history[src_ip].append(event)
        
        # Keep only last 24 hours
        cutoff = time.time() - 86400
        self.ip_history[src_ip] = [e for e in self.ip_history[src_ip] if e.get('timestamp', 0) > cutoff]
    
    def _calculate_failure_rate(self, events: List[Dict[str, Any]]) -> float:
        """Calculate ratio of failed events"""
        if not events:
            return 0.0
        
        failed_events = [e for e in events if 'failed' in e.get('event_type', '')]
        return len(failed_events) / len(events)
    
    def _is_internal_ip(self, ip: str) -> bool:
        """Check if IP is internal/private"""
        if ip.startswith('192.168.') or ip.startswith('10.') or ip.startswith('172.16.'):
            return True
        return False
    
    def _is_new_ip_for_user(self, user_id: str, src_ip: str) -> bool:
        """Check if this is a new IP for the user"""
        user_events = self.user_history.get(user_id, [])
        user_ips = set(e.get('src_ip') for e in user_events)
        return src_ip not in user_ips
