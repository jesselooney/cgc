from pathlib import Path
import subprocess
import sys

GOLD_DIR = Path("test") / "gold"
GOLDENS_DIR = GOLD_DIR / "goldens"
NEW_GOLDENS_DIR = GOLD_DIR / "new_goldens"

if not GOLDENS_DIR.is_dir():
    print("[ERROR] Goldens directory does not exist.")
    sys.exit(1)

# TODO: Maybe clean NEW_GOLDENS_DIR?

subprocess.run(["make", f"GOLD_OUT={str(NEW_GOLDENS_DIR)}", "gold_generate"])

test_count = 0
success_count = 0

for golden in GOLDENS_DIR.iterdir():
    if golden.is_file():
        test_count += 1

        new_golden = NEW_GOLDENS_DIR / golden.name
        if not new_golden.is_file():
            print(f"[ERROR] No output to test against '{golden.name}' test")
            continue

        completed_process = subprocess.run(["diff", str(golden), str(new_golden)], capture_output=True, text=True)

        if completed_process.returncode == 1:
            # New golden differs from the existing one.
            print(f"[ERROR] Golden test '{golden.name}' failed.")
            print(completed_process.stdout)
            print("==========================================")
            continue

        # If we got here, the test succeeded, so we can delete the new golden.
        new_golden.unlink()
        success_count += 1

print(f"{success_count}/{test_count} tests passed.")
if success_count < test_count:
    print("[ERROR] Some tests failed!")
    print(f"New goldens for failed tests are preserved in {str(NEW_GOLDENS_DIR)}")
    sys.exit(1)
else:
    # Directory should be empty since all tests passed.
    NEW_GOLDENS_DIR.rmdir()
