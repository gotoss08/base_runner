# generate_version.py
import sys
import subprocess
from datetime import datetime, UTC

template_path = sys.argv[1]
output_path = sys.argv[2]
project_name = sys.argv[3]
project_version = sys.argv[4]

try:
    commit_hash = subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode().strip()
except Exception:
    commit_hash = "unknown"

build_datetime = datetime.now(UTC).strftime("%Y-%m-%dT%H:%M:%SZ")

with open(template_path, 'r') as f:
    content = f.read()

content = content.replace("@PROJECT_NAME@", project_name)
content = content.replace("@PROJECT_VERSION@", project_version)

major, minor, patch = project_version.split(".")
content = content.replace("@PROJECT_VERSION_MAJOR@", major)
content = content.replace("@PROJECT_VERSION_MINOR@", minor)
content = content.replace("@PROJECT_VERSION_PATCH@", patch)

content = content.replace("@GIT_COMMIT@", commit_hash)
content = content.replace("@GIT_COMMIT_HASH@", commit_hash[:7])
content = content.replace("@BUILD_DATETIME@", build_datetime)

with open(output_path, 'w') as f:
    f.write(content)
