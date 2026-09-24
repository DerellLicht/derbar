# der_libs\release.mak -- shared GitHub release plumbing, included by each
# project's top-level Makefile. Assumes CHANGELOG.md lives in the project
# root with "## [x.y]" version headers, and that "gh" (GitHub CLI) is
# authenticated for this repo. GitHub-only by design -- PrettyReMark stays
# on GitLab as a standalone exception (fork continuity) and is not part of
# this include.
#
# Usage in a project Makefile:
#   include der_libs\release.mak
#   ...
#   DIST_ZIP := $(BASE)V$(VERSION).zip
#   # RELEASE_ASSETS defaults to "./$(DIST_ZIP) ./CHANGELOG.md" below;
#   # override after the include only if a project ships something else.
#
#   dist:
#   	rm -f *.zip
#   	zip $(DIST_ZIP) $(BINX) readme.md LICENSE.txt CHANGELOG.md
#
# That's all a project needs to supply -- check-clean, notes, release,
# update, retag, re-release, and sha256 all come from here.

# Most recent "## [x.y]" header in CHANGELOG.md, e.g. "## [1.16]" -> 1.16
VERSION := $(shell grep -oE '\[[0-9]+\.[0-9]+\]' CHANGELOG.md | head -n 1 | tr -d '[]')
TAG := v$(VERSION)

# Default asset list for release/update. Recursively expanded (plain '?='),
# so it's safe to reference here even though DIST_ZIP is normally defined
# by the project Makefile *after* this include -- it's only resolved when
# release/update actually run, by which point DIST_ZIP is set.
RELEASE_ASSETS ?= ./$(DIST_ZIP) ./CHANGELOG.md

.PHONY: check-clean notes release update retag re-release sha256

# Blocks release/retag on an uncommitted working tree -- catches building
# from a tree that doesn't match what the tag is about to point at. Runs
# before the (slow) dist rebuild.
check-clean:
	@if [ -n "$$(git status --porcelain)" ]; then \
		echo "ERROR: uncommitted changes present -- commit before releasing."; \
		git status --short; \
		exit 1; \
	fi

# Slices this version's section out of CHANGELOG.md into temp_notes.md, for
# release/update to hand to gh via --notes-file.
notes:
	sed -n '/## \[$(VERSION)\]/,/## \[/p' CHANGELOG.md | sed '$$d' > temp_notes.md

release: check-clean dist notes
	@cmd /C "@echo Preparing GitHub release for $(TAG)..."
	gh release create $(TAG) $(RELEASE_ASSETS) --notes-file temp_notes.md
	rm temp_notes.md
	@cmd /C "@echo Release $(TAG) successfully uploaded to GitHub!"

# Notes first, then assets ("--clobber" makes the asset re-upload safe with
# no stale-link cleanup needed). "gh release edit" fails if the release
# doesn't exist yet -- unlike "release" this is NOT self-healing, so run
# "release" first for a tag's first publish.
update: dist notes
	@cmd /C "@echo Updating release $(TAG)..."
	gh release edit $(TAG) --notes-file temp_notes.md
	rm temp_notes.md
	gh release upload $(TAG) $(RELEASE_ASSETS) --clobber
	@cmd /C "@echo Release $(TAG) assets and notes successfully updated on GitHub!"

# Recovery for "released with pending changes left out of the tag":
# force-moves $(TAG) to HEAD and re-pushes, then "update" re-releases
# against it. Solo-repo only -- force-pushing a moved tag is unsafe if
# anyone else has already fetched it. Deliberately separate from "release",
# which lets gh create the tag itself (no explicit git tag), so a plain
# release never force-moves anything.
retag: check-clean
	@if git rev-parse $(TAG) >/dev/null 2>&1; then \
		echo "Retagging existing tag $(TAG)."; \
	else \
		echo "Note: $(TAG) doesn't exist yet -- this will be its first release."; \
	fi
	git tag -f $(TAG)
	git push origin $(TAG) --force

re-release: retag update
	@cmd /C "@echo Release $(TAG) retagged and re-released."

sha256:
	certutil -hashfile $(DIST_ZIP) SHA256
