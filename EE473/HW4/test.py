import numpy as np
import matplotlib.pyplot as plt

# 1. System parameters
a1, a2 = 1.0, 1.0
t = np.linspace(0, 10, 500)

# 2. Calculate all angles for all points in time at once
theta1 = 0.0 + (1.0 * t)
theta2 = (np.pi / 2) + (-1.0 * t)

# 3. Calculate all Jacobian components at once
s1 = np.sin(theta1)
c1 = np.cos(theta1)
s12 = np.sin(theta1 + theta2)
c12 = np.cos(theta1 + theta2)

# 4. V = J * q_dot
vx = (-a1*s1 - a2*s12)*(1.0) + (-a2*s12)*(-1.0)
vy = ( a1*c1 + a2*c12)*(1.0) + ( a2*c12)*(-1.0)
wz = np.zeros_like(t)

# 5. Plot
plt.figure(figsize=(10, 6))
plt.plot(t, vx, label='Linear Velocity X ($v_x$)', color='blue', linewidth=2)
plt.plot(t, vy, label='Linear Velocity Y ($v_y$)', color='red', linewidth=2)
plt.plot(t, wz, label='Angular Velocity Z ($\omega_z$)', color='green', linestyle='--', linewidth=2)

# --- AXIS LABELS AND TITLE ---
plt.title('End-Effector Velocities Over Time')
plt.xlabel('Time (seconds)')
plt.ylabel('Velocity (m/s and rad/s)')
# -----------------------------

plt.axhline(0, color='black', linewidth=1)
plt.grid(True, linestyle=':', alpha=0.7)
plt.legend(loc='upper right')
plt.tight_layout()
plt.show()
