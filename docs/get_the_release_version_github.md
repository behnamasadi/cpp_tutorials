To get the latest release version of a GitHub repository using `curl`, you can query the GitHub API. Here's how you can do it:

1. Open a terminal or command prompt.
2. Run the following `curl` command, replacing `owner` with the repository owner's name and `repo` with the repository's name:

```bash
curl --silent "https://api.github.com/repos/owner/repo/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/'
```

### Explanation:
- `https://api.github.com/repos/owner/repo/releases/latest`: This is the GitHub API endpoint for getting the latest release of a repository.
- `grep '"tag_name":'`: Filters the response to show only the line that contains the release tag.
- `sed -E 's/.*"([^"]+)".*/\1/'`: Extracts the version tag (the part inside double quotes after `"tag_name":`).

### Example:
If you want to get the latest release of the repository `behnamasadi/cpp_tutorials`, you would run:

```bash
curl --silent "https://api.github.com/repos/behnamasadi/cpp_tutorials/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/'
```

for instance:

```
curl --silent "https://api.github.com/repos/nerfstudio-project/gsplat/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/'
```


