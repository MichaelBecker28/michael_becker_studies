import logging
import signal
import sys
import time
from src.collectors.test_collector import TestLogCollector

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

def signal_handler(sig, frame):
    print('\n🛑 Stopping test collector...')
    collector.stop()
    sys.exit(0)

if __name__ == "__main__":
    print("🎯 AI Threat Detection System - Test Mode")
    print("=" * 50)
    
    # Create test collector
    config = {'source_name': 'test_generator'}
    collector = TestLogCollector(config)
    
    # Setup signal handler for Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)
    
    print("Press Ctrl+C to stop")
    print("-" * 50)
    
    try:
        collector.start()
    except KeyboardInterrupt:
        collector.stop()
        print("\n✅ Test completed!")
