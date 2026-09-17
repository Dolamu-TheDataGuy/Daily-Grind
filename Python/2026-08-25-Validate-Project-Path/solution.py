import os


def is_path_allowed(project_dir, requested_path):
    norm_project_path = os.path.abspath(os.path.normpath(project_dir))
    if os.path.isabs(requested_path):
        requested_path = os.path.abspath(os.path.normpath(requested_path))
    else:
        requested_path = os.path.abspath(
            os.path.normpath(os.path.join(norm_project_path, requested_path))
        )

    try:
        if os.path.commonpath([norm_project_path, requested_path]) == norm_project_path:
            return True
        return False

    except ValueError:
        return False
