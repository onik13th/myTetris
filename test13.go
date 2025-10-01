// convert/pull_request_files.go
package convert

import (
	"strings"

	"code.gitea.io/gitea/models/issues"
	"code.gitea.io/gitea/routers/publicapi/response"
	"code.gitea.io/gitea/routers/publicapi/util"
	"code.gitea.io/gitea/services/context"
	"code.gitea.io/gitea/services/gitdiff"
)

// ToPullRequestFile конвертирует ОДИН DiffFile в CommitFiles
func ToPullRequestFile(ctx *context.Context, df *gitdiff.DiffFile, pull *issues.PullRequest, afterCommitID string) (*response.CommitFiles, error) {
	blobURL := util.GetBlobURL(ctx.Repo.Owner.Name, ctx.Repo.Repository.Name, afterCommitID, df.Name)
	rawURL := util.GetRawURL(ctx.Repo.Owner.Name, ctx.Repo.Repository.Name, afterCommitID, df.Name)
	contentsURL := util.GetContentsURL(ctx.Repo.Owner.Name, ctx.Repo.Repository.Name, afterCommitID, df.Name)

	return &response.CommitFiles{
		SHA:         df.NameHash,
		Filename:    df.Name,
		Status:      getFileStatus(df),
		Additions:   df.Addition,
		Deletions:   df.Deletion,
		Changes:     df.Addition + df.Deletion,
		BlobURL:     blobURL,
		RawURL:      rawURL,
		ContentsURL: contentsURL,
		Patch:       buildPatch(df),
	}, nil
}

// getFileStatus определяет статус файла
func getFileStatus(df *gitdiff.DiffFile) string {
	if df.IsCreated {
		return "added"
	}
	if df.IsDeleted {
		return "removed"
	}
	if df.IsRenamed {
		return "renamed"
	}
	return "modified"
}

// buildPatch строит патч из секций
func buildPatch(df *gitdiff.DiffFile) string {
	if len(df.Sections) == 0 {
		return ""
	}

	var patch strings.Builder
	for _, section := range df.Sections {
		for _, line := range section.Lines {
			patch.WriteString(line.Content)
			patch.WriteString("\n")
		}
	}
	return patch.String()
}
