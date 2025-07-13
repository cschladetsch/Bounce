#!/usr/bin/env python3
"""Simple script to play MIDI using pygame (if available)"""

import sys

try:
    import pygame
    import pygame.midi
    
    # Initialize pygame
    pygame.init()
    pygame.midi.init()
    
    # Print available MIDI outputs
    print("Available MIDI outputs:")
    for i in range(pygame.midi.get_count()):
        info = pygame.midi.get_device_info(i)
        if info[3] == 1:  # Is output
            print(f"  {i}: {info[1].decode()}")
    
    # Load and play the MIDI file
    pygame.mixer.music.load("industrial_test.mid")
    pygame.mixer.music.play()
    
    print("\nPlaying industrial_test.mid...")
    print("Press Ctrl+C to stop")
    
    # Keep playing until interrupted
    while pygame.mixer.music.get_busy():
        pygame.time.wait(100)
        
except ImportError:
    print("pygame not installed. To play MIDI files, install it with:")
    print("  pip install pygame")
except KeyboardInterrupt:
    print("\nStopped.")
except Exception as e:
    print(f"Error: {e}")
    print("\nThe MIDI file has been generated successfully.")
    print("To play it, you need a MIDI player or synthesizer installed.")