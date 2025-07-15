# Leveling protocol

## Final Version

Leveler is linked to axis

- Rpi sends level command
- Printer parses which axis to level
- Calls axis leveler
- Leveler moves axis/other required axis
- Tells RPI it's done/activates the callback from before

Benefits:

- Can level multiple axis at once
- If callback, Leveler doesn't need special privelages
