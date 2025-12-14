import re
import numpy as np

COMMAND_RE = re.compile(r'([MmLlHhVv])|(-?\d+\.?\d*)')

def parse_path(d):
    tokens = COMMAND_RE.findall(d)
    tokens = [t[0] or t[1] for t in tokens]

    points = []
    x = y = 0
    i = 0
    cmd = None

    while i < len(tokens):
        t = tokens[i]

        if t.isalpha():
            cmd = t
            i += 1
            continue

        if cmd == 'M':   # absolute moveto
            x, y = float(t), float(tokens[i+1])
            points.append((x, y))
            i += 2

        elif cmd == 'm': # relative moveto
            x += float(t)
            y += float(tokens[i+1])
            points.append((x, y))
            i += 2

        elif cmd == 'L': # absolute line
            x, y = float(t), float(tokens[i+1])
            points.append((x, y))
            i += 2

        elif cmd == 'l': # relative line
            x += float(t)
            y += float(tokens[i+1])
            points.append((x, y))
            i += 2

        elif cmd == 'H': # absolute horizontal
            x = float(t)
            points.append((x, y))
            i += 1

        elif cmd == 'h': # relative horizontal
            x += float(t)
            points.append((x, y))
            i += 1

        elif cmd == 'V': # absolute vertical
            y = float(t)
            points.append((x, y))
            i += 1

        elif cmd == 'v': # relative vertical
            y += float(t)
            points.append((x, y))
            i += 1

        else:
            raise ValueError(f"Unsupported command: {cmd}")

    return np.array(points)


# ---------- Normalization ----------

def normalize_points(points, adv_width, height):
    pts = np.array(points, dtype=float)

    # X: use horiz-adv-x
    pts[:, 0] = 2 * (pts[:, 0] / adv_width) - 1

    # Y: use font height
    pts[:, 1] = 2 * (pts[:, 1] / height) - 1

    return pts


d = """
M 126 0
L 445 666
L 445 670
L 40 670
"""
horiz_adv_x = 495
height = 670

raw_points = parse_path(d)
norm_points = normalize_points(raw_points, horiz_adv_x, height)

print("Raw points:")
print(raw_points)

print("\nNormalized points (-1..1):")
for pt in np.round(norm_points, 3):
    print(f"Coordinate({pt[0]:6.3f}, {pt[1]:6.3f}),")

print("\nNumber of points:", len(norm_points))