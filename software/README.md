# Software

The software side contains the Windows desktop USB voice client.

## Desktop voice client

Path:

`desktop_usb_voice/`

It uses:

- Python
- pyserial
- Tkinter
- pyttsx3

Install dependencies from the repository root:

```cmd
python -m pip install -r software\desktop_usb_voice\requirements.txt
```

Run:

```cmd
python software\desktop_usb_voice\sign_language_usb_voice.py
```

The client reads Arduino serial data at 9600 baud and speaks detected `SIGN:` messages.
