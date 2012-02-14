#include "ignore.h"
static int file_delta_new__from_one(
	git_diff_list *diff,
	git_status_t   status,
	mode_t         attr,
	const git_oid *oid,
	const char    *path)
	git_diff_delta *delta = git__calloc(1, sizeof(git_diff_delta));
	/* This fn is just for single-sided diffs */
	assert(status == GIT_STATUS_ADDED || status == GIT_STATUS_DELETED);

	if (!delta)
		return git__rethrow(GIT_ENOMEM, "Could not allocate diff record");
	if ((delta->path = git__strdup(path)) == NULL) {
		git__free(delta);
		return git__rethrow(GIT_ENOMEM, "Could not allocate diff record path");
	}
	if (diff->opts.flags & GIT_DIFF_REVERSE)
		status = (status == GIT_STATUS_ADDED) ?
			GIT_STATUS_DELETED : GIT_STATUS_ADDED;

	delta->status = status;
	if (status == GIT_STATUS_ADDED) {
		delta->new_attr = attr;
		if (oid != NULL)
			git_oid_cpy(&delta->new_oid, oid);
	} else {
		delta->old_attr = attr;
		if (oid != NULL)
			git_oid_cpy(&delta->old_oid, oid);
	}

	if ((error = git_vector_insert(&diff->files, delta)) < GIT_SUCCESS)
	return error;
}

static int file_delta_new__from_tree_diff(
	git_diff_list *diff,
	const git_tree_diff_data *tdiff)
{
	int error;
	git_diff_delta *delta = git__calloc(1, sizeof(git_diff_delta));

	if (!delta)
		return git__rethrow(GIT_ENOMEM, "Could not allocate diff record");

	if ((diff->opts.flags & GIT_DIFF_REVERSE) == 0) {
		delta->status   = tdiff->status;
		delta->old_attr = tdiff->old_attr;
		delta->new_attr = tdiff->new_attr;
		delta->old_oid  = tdiff->old_oid;
		delta->new_oid  = tdiff->new_oid;
	} else {
		/* reverse the polarity of the neutron flow */
		switch (tdiff->status) {
		case GIT_STATUS_ADDED:   delta->status = GIT_STATUS_DELETED; break;
		case GIT_STATUS_DELETED: delta->status = GIT_STATUS_ADDED; break;
		default:                 delta->status = tdiff->status;
		}
		delta->old_attr = tdiff->new_attr;
		delta->new_attr = tdiff->old_attr;
		delta->old_oid  = tdiff->new_oid;
		delta->new_oid  = tdiff->old_oid;
	}

	delta->path = git__strdup(diff->pfx.ptr);
	if (delta->path == NULL) {
		git__free(delta);
		return git__rethrow(GIT_ENOMEM, "Could not allocate diff record path");
	}

	if ((error = git_vector_insert(&diff->files, delta)) < GIT_SUCCESS)
		file_delta_free(delta);
static int create_diff_for_tree_entry(const char *root, git_tree_entry *entry, void *data)
	if (S_ISDIR(git_tree_entry_attributes(entry)))
	/* join pfx, root, and entry->filename into one */
		(error = git_buf_joinpath(
			&diff->pfx, diff->pfx.ptr, git_tree_entry_name(entry))))
	error = file_delta_new__from_one(
		diff, diff->status, git_tree_entry_attributes(entry),
		git_tree_entry_id(entry), diff->pfx.ptr);
static int tree_to_tree_diff_cb(const git_tree_diff_data *tdiff, void *data)
	error = git_buf_joinpath(&diff->pfx, diff->pfx.ptr, tdiff->path);
	if (S_ISDIR(tdiff->old_attr) && S_ISDIR(tdiff->new_attr)) {
		if (!(error = git_tree_lookup(&old, diff->repo, &tdiff->old_oid)) &&
			!(error = git_tree_lookup(&new, diff->repo, &tdiff->new_oid)))
			error = git_tree_diff(old, new, tree_to_tree_diff_cb, diff);
	} else if (S_ISDIR(tdiff->old_attr) || S_ISDIR(tdiff->new_attr)) {
		git_tree *tree     = NULL;
		int added_dir      = S_ISDIR(tdiff->new_attr);
		const git_oid *oid = added_dir ? &tdiff->new_oid : &tdiff->old_oid;
		diff->status       = added_dir ? GIT_STATUS_ADDED : GIT_STATUS_DELETED;

		if (!(error = git_tree_lookup(&tree, diff->repo, oid)))
			error = git_tree_walk(
				tree, create_diff_for_tree_entry, GIT_TREEWALK_POST, diff);
		git_tree_free(tree);
	} else
		error = file_delta_new__from_tree_diff(diff, tdiff);
static char *git_diff_src_prefix_default = "a/";
static char *git_diff_dst_prefix_default = "b/";
#define PREFIX_IS_DEFAULT(A) \
	((A) == git_diff_src_prefix_default || (A) == git_diff_dst_prefix_default)

static char *copy_prefix(const char *prefix)
{
	size_t len = strlen(prefix);
	char *str = git__malloc(len + 2);
	if (str != NULL) {
		memcpy(str, prefix, len + 1);
		/* append '/' at end if needed */
		if (len > 0 && str[len - 1] != '/') {
			str[len] = '/';
			str[len + 1] = '\0';
		}
	}
	return str;
}

	if (diff == NULL)
		return NULL;

	diff->repo = repo;
	git_buf_init(&diff->pfx, 0);

	if (opts == NULL)
		return diff;

	memcpy(&diff->opts, opts, sizeof(git_diff_options));

	diff->opts.src_prefix = (opts->src_prefix == NULL) ?
		git_diff_src_prefix_default : copy_prefix(opts->src_prefix);
	diff->opts.dst_prefix = (opts->dst_prefix == NULL) ?
		git_diff_dst_prefix_default : copy_prefix(opts->dst_prefix);
	if (!diff->opts.src_prefix || !diff->opts.dst_prefix) {
		git__free(diff);
		return NULL;
	}
	if (diff->opts.flags & GIT_DIFF_REVERSE) {
		char *swap = diff->opts.src_prefix;
		diff->opts.src_prefix = diff->opts.dst_prefix;
		diff->opts.dst_prefix = swap;

	/* do something safe with the pathspec strarray */

	if (!PREFIX_IS_DEFAULT(diff->opts.src_prefix)) {
		git__free(diff->opts.src_prefix);
		diff->opts.src_prefix = NULL;
	}
	if (!PREFIX_IS_DEFAULT(diff->opts.dst_prefix)) {
		git__free(diff->opts.dst_prefix);
		diff->opts.dst_prefix = NULL;
	}
	error = git_tree_diff(old, new, tree_to_tree_diff_cb, diff);
	if (error == GIT_SUCCESS) {
typedef struct {
	git_diff_list *diff;
	git_index     *index;
	unsigned int  index_pos;
	git_ignores   *ignores;
} diff_callback_info;

static int add_new_index_deltas(
	diff_callback_info *info,
	git_status_t status,
	const char *stop_path)
{
	int error;
	git_index_entry *idx_entry = git_index_get(info->index, info->index_pos);

	while (idx_entry != NULL &&
		(stop_path == NULL || strcmp(idx_entry->path, stop_path) < 0))
	{
		error = file_delta_new__from_one(
			info->diff, status, idx_entry->mode,
			&idx_entry->oid, idx_entry->path);
		if (error < GIT_SUCCESS)
			return error;

		idx_entry = git_index_get(info->index, ++info->index_pos);
	}

	return GIT_SUCCESS;
}

static int diff_index_to_tree_cb(const char *root, git_tree_entry *tree_entry, void *data)
{
	int error;
	diff_callback_info *info = data;
	git_index_entry *idx_entry;

	/* TODO: submodule support for GIT_OBJ_COMMITs in tree */
	if (git_tree_entry_type(tree_entry) != GIT_OBJ_BLOB)
		return GIT_SUCCESS;

	error = git_buf_joinpath(&info->diff->pfx, root, git_tree_entry_name(tree_entry));
	if (error < GIT_SUCCESS)
		return error;

	/* create add deltas for index entries that are not in the tree */
	error = add_new_index_deltas(info, GIT_STATUS_ADDED, info->diff->pfx.ptr);
	if (error < GIT_SUCCESS)
		return error;

	/* create delete delta for tree entries that are not in the index */
	idx_entry = git_index_get(info->index, info->index_pos);
	if (idx_entry == NULL || strcmp(idx_entry->path, info->diff->pfx.ptr) > 0) {
		return file_delta_new__from_one(
			info->diff, GIT_STATUS_DELETED, git_tree_entry_attributes(tree_entry),
			git_tree_entry_id(tree_entry), info->diff->pfx.ptr);
	}

	/* create modified delta for non-matching tree & index entries */
	info->index_pos++;

	if (git_oid_cmp(&idx_entry->oid, git_tree_entry_id(tree_entry)) ||
		idx_entry->mode != git_tree_entry_attributes(tree_entry))
	{
		git_tree_diff_data tdiff;
		tdiff.old_attr = git_tree_entry_attributes(tree_entry);
		tdiff.new_attr = idx_entry->mode;
		tdiff.status   = GIT_STATUS_MODIFIED;
		tdiff.path     = idx_entry->path;
		git_oid_cpy(&tdiff.old_oid, git_tree_entry_id(tree_entry));
		git_oid_cpy(&tdiff.new_oid, &idx_entry->oid);

		error = file_delta_new__from_tree_diff(info->diff, &tdiff);
	}

	return error;

}

int git_diff_index_to_tree(
	git_repository *repo,
	const git_diff_options *opts,
	git_tree *old,
	git_diff_list **diff_ptr)
{
	int error;
	diff_callback_info info = {0};

	if ((info.diff = git_diff_list_alloc(repo, opts)) == NULL)
		return GIT_ENOMEM;

	if ((error = git_repository_index(&info.index, repo)) == GIT_SUCCESS) {
		error = git_tree_walk(
			old, diff_index_to_tree_cb, GIT_TREEWALK_POST, &info);
		if (error == GIT_SUCCESS)
			error = add_new_index_deltas(&info, GIT_STATUS_ADDED, NULL);
		git_index_free(info.index);
	}
	git_buf_free(&info.diff->pfx);

	if (error != GIT_SUCCESS)
		git_diff_list_free(info.diff);
	else
		*diff_ptr = info.diff;

	return error;
}

typedef struct {
	struct stat st;
	mode_t mode;
	char path[GIT_FLEX_ARRAY];
} workdir_entry;

#define MODE_PERMS_MASK 0777

/* TODO: need equiv of core git's "trust_executable_bit" flag? */
#define CANONICAL_PERMS(MODE) (((MODE) & 0100) ? 0755 : 0644)
#define MODE_TYPE(MODE)  ((MODE) & ~MODE_PERMS_MASK)

static mode_t canonical_mode(mode_t raw_mode)
{
	if (S_ISREG(raw_mode))
		return S_IFREG | CANONICAL_PERMS(raw_mode);
	else if (S_ISLNK(raw_mode))
		return S_IFLNK;
	else if (S_ISDIR(raw_mode))
		return S_IFDIR;
	else if (S_ISGITLINK(raw_mode))
		return S_IFGITLINK;
	else
		return 0;
}

static int diff_workdir_insert(void *data, git_buf *dir)
{
	workdir_entry *wd_entry = git__malloc(sizeof(workdir_entry) + dir->size + 2);
	if (wd_entry == NULL)
		return GIT_ENOMEM;
	if (p_lstat(dir->ptr, &wd_entry->st) < 0) {
		git__free(wd_entry);
		return GIT_EOSERR;
	}
	git_buf_copy_cstr(wd_entry->path, dir->size + 1, dir);
	wd_entry->mode = canonical_mode(wd_entry->st.st_mode);
	/* suffix directories with / to mimic tree/index sort order */
	if (S_ISDIR(wd_entry->st.st_mode)) {
		wd_entry->path[dir->size] = '/';
		wd_entry->path[dir->size+1] = '\0';
	}

	return git_vector_insert((git_vector *)data, wd_entry);
}

static int diff_workdir_walk(
	const char *dir,
	diff_callback_info *info,
	int (*cb)(diff_callback_info *, workdir_entry *))
{
	int error = GIT_SUCCESS;
	git_vector files = GIT_VECTOR_INIT;
	git_buf buf = GIT_BUF_INIT;
	unsigned int i;
	workdir_entry *wd_entry;
	git_ignores ignores = {0}, *old_ignores = info->ignores;

	if (!dir)
		dir = git_repository_workdir(info->diff->repo);

	if ((error = git_vector_init(&files, 0, git__strcmp_cb)) < GIT_SUCCESS ||
		(error = git_buf_sets(&buf, dir)) < GIT_SUCCESS ||
		(error = git_path_direach(&buf, diff_workdir_insert, &files)) < GIT_SUCCESS ||
		(error = git_ignore__for_path(info->diff->repo, dir, &ignores)) < GIT_SUCCESS)
		goto cleanup;

	git_vector_sort(&files);
	info->ignores = old_ignores;

	git_vector_foreach(&files, i, wd_entry) {
		if ((error = cb(info, wd_entry)) < GIT_SUCCESS)
			goto cleanup;
	}

cleanup:
	git_vector_foreach(&files, i, wd_entry)
		git__free(wd_entry);
	info->ignores = old_ignores;
	git_ignore__free(&ignores);
	git_vector_free(&files);
	git_buf_free(&buf);

	return error;
}

static int found_new_workdir_entry(
	diff_callback_info *info, workdir_entry *wd_entry)
{
	int error;
	int ignored = 0;
	git_status_t status;

	/* skip file types that are not trackable */
	if (wd_entry->mode == 0)
		return GIT_SUCCESS;

	error = git_ignore__lookup(info->ignores, wd_entry->path, &ignored);
	if (error < GIT_SUCCESS)
		return error;
	status = ignored ? GIT_STATUS_IGNORED : GIT_STATUS_UNTRACKED;

	return file_delta_new__from_one(
		info->diff, status, wd_entry->mode, NULL, wd_entry->path);
}

static int diff_workdir_to_index_cb(
	diff_callback_info *info, workdir_entry *wd_entry)
{
	int error, modified;
	git_index_entry *idx_entry;
	git_oid new_oid;

	/* Store index entries that precede this workdir entry */
	error = add_new_index_deltas(info, GIT_STATUS_DELETED, wd_entry->path);
	if (error < GIT_SUCCESS)
		return error;

	/* Process workdir entries that are not in the index.
	 * These might be untracked, ignored, or special (dirs, etc).
	 */
	idx_entry = git_index_get(info->index, info->index_pos);
	if (idx_entry == NULL || strcmp(idx_entry->path, wd_entry->path) > 0) {
		git_buf dotgit = GIT_BUF_INIT;
		int contains_dotgit;

		if (!S_ISDIR(wd_entry->mode))
			return found_new_workdir_entry(info, wd_entry);

		error = git_buf_joinpath(&dotgit, wd_entry->path, DOT_GIT);
		if (error < GIT_SUCCESS)
			return error;
		contains_dotgit = (git_path_exists(dotgit.ptr) == GIT_SUCCESS);
		git_buf_free(&dotgit);

		if (contains_dotgit)
			/* TODO: deal with submodule or embedded repo */
			return GIT_SUCCESS;
		else if (git__prefixcmp(idx_entry->path, wd_entry->path) == GIT_SUCCESS)
			/* there are entries in the directory in the index already,
			 * so recurse into it.
			 */
			return diff_workdir_walk(wd_entry->path, info, diff_workdir_to_index_cb);
		else
			/* TODO: this is not the same behavior as core git.
			 *
			 * I don't recurse into the directory once I know that no files
			 * in it are being tracked.  But core git does and only adds an
			 * entry if there are non-directory entries contained under the
			 * dir (although, interestingly, it only shows the dir, not the
			 * individual entries).
			 */
			return found_new_workdir_entry(info, wd_entry);
	}

	/* create modified delta for non-matching tree & index entries */
	info->index_pos++;

	/* check for symlink/blob changes and split into add/del pair */
	if (MODE_TYPE(wd_entry->mode) != MODE_TYPE(idx_entry->mode)) {
		error = file_delta_new__from_one(
			info->diff, GIT_STATUS_DELETED,
			idx_entry->mode, &idx_entry->oid, idx_entry->path);
		if (error < GIT_SUCCESS)
			return error;

		/* because of trailing slash, cannot have non-dir to dir transform */
		assert(!S_ISDIR(wd_entry->mode));

		return file_delta_new__from_one(
			info->diff, GIT_STATUS_ADDED,
			wd_entry->mode, NULL, wd_entry->path);
	}

	/* mode or size changed, so git blob has definitely changed */
	if (wd_entry->mode != idx_entry->mode ||
		wd_entry->st.st_size != idx_entry->file_size)
	{
		modified = 1;
		memset(&new_oid, 0, sizeof(new_oid));
	}

	/* all other things are indicators there might be a change, so get oid */
	if (!modified &&
		((git_time_t)wd_entry->st.st_ctime != idx_entry->ctime.seconds ||
		 (git_time_t)wd_entry->st.st_mtime != idx_entry->mtime.seconds ||
		 (unsigned int)wd_entry->st.st_dev != idx_entry->dev ||
		 (unsigned int)wd_entry->st.st_ino != idx_entry->ino ||
		 /* TODO: need TRUST_UID_GID configs */
		 (unsigned int)wd_entry->st.st_uid != idx_entry->uid ||
		 (unsigned int)wd_entry->st.st_gid != idx_entry->gid))
	{
		/* calculate oid to confirm change */
		if (S_ISLNK(wd_entry->st.st_mode))
			error = git_odb__hashlink(&new_oid, wd_entry->path);
		else {
			int fd;
			if ((fd = p_open(wd_entry->path, O_RDONLY)) < 0)
				error = git__throw(
					GIT_EOSERR, "Could not open '%s'", wd_entry->path);
			else {
				error = git_odb__hashfd(
					&new_oid, fd, wd_entry->st.st_size, GIT_OBJ_BLOB);
				p_close(fd);
			}
		}

		if (error < GIT_SUCCESS)
			return error;

		modified = (git_oid_cmp(&new_oid, &idx_entry->oid) != 0);
	}

	/* TODO: check index flags for forced ignore changes */

	if (modified) {
		git_tree_diff_data tdiff;

		tdiff.old_attr = idx_entry->mode;
		tdiff.new_attr = wd_entry->mode;
		tdiff.status   = GIT_STATUS_MODIFIED;
		tdiff.path     = wd_entry->path;
		git_oid_cpy(&tdiff.old_oid, &idx_entry->oid);
		git_oid_cpy(&tdiff.new_oid, &new_oid);

		error = file_delta_new__from_tree_diff(info->diff, &tdiff);
	}

	return error;
}

int git_diff_workdir_to_index(
	git_repository *repo,
	const git_diff_options *opts,
	git_diff_list **diff)
{
	int error;
	diff_callback_info info = {0};

	if ((info.diff = git_diff_list_alloc(repo, opts)) == NULL)
		return GIT_ENOMEM;

	if ((error = git_repository_index(&info.index, repo)) == GIT_SUCCESS) {
		error = diff_workdir_walk(NULL, &info, diff_workdir_to_index_cb);
		if (error == GIT_SUCCESS)
			error = add_new_index_deltas(&info, GIT_STATUS_DELETED, NULL);
		git_index_free(info.index);
	}
	git_buf_free(&info.diff->pfx);

	if (error != GIT_SUCCESS)
		git_diff_list_free(info.diff);
	else
		*diff = info.diff;

	return error;
}

} diff_output_info;
	diff_output_info *info = priv;
	if (len == 1 && info->hunk_cb) {
				err = info->hunk_cb(
					info->cb_data, info->delta, &range, bufs[0].ptr, bufs[0].size);
	else if ((len == 2 || len == 3) && info->line_cb) {
		err = info->line_cb(
			info->cb_data, info->delta, origin, bufs[1].ptr, bufs[1].size);
			err = info->line_cb(
				info->cb_data, info->delta, origin, bufs[2].ptr, bufs[2].size);
static void setup_xdiff_options(
	git_diff_options *opts, xdemitconf_t *cfg, xpparam_t *param)
{
	memset(cfg, 0, sizeof(xdemitconf_t));
	memset(param, 0, sizeof(xpparam_t));

	cfg->ctxlen =
		(!opts || !opts->context_lines) ? 3 : opts->context_lines;
	cfg->interhunkctxlen =
		(!opts || !opts->interhunk_lines) ? 3 : opts->interhunk_lines;

	if (!opts)
		return;

	if (opts->flags & GIT_DIFF_IGNORE_WHITESPACE)
		param->flags |= XDF_WHITESPACE_FLAGS;
	if (opts->flags & GIT_DIFF_IGNORE_WHITESPACE_CHANGE)
		param->flags |= XDF_IGNORE_WHITESPACE_CHANGE;
	if (opts->flags & GIT_DIFF_IGNORE_WHITESPACE_EOL)
		param->flags |= XDF_IGNORE_WHITESPACE_AT_EOL;
}

	diff_output_info info;
	xpparam_t    xdiff_params;
	xdemitconf_t xdiff_config;
	xdemitcb_t   xdiff_callback;

	info.diff    = diff;
	info.cb_data = data;
	info.hunk_cb = hunk_cb;
	info.line_cb = line_cb;
	setup_xdiff_options(&diff->opts, &xdiff_config, &xdiff_params);
	memset(&xdiff_callback, 0, sizeof(xdiff_callback));
	xdiff_callback.outf = diff_output_cb;
	xdiff_callback.priv = &info;
	git_vector_foreach(&diff->files, info.index, delta) {
		mmfile_t old_data, new_data;
				old_data.ptr = (char *)git_blob_rawcontent(delta->old_blob);
				old_data.size = git_blob_rawsize(delta->old_blob);
				old_data.ptr = "";
				old_data.size = 0;
				new_data.ptr = (char *)git_blob_rawcontent(delta->new_blob);
				new_data.size = git_blob_rawsize(delta->new_blob);
				new_data.ptr = "";
				new_data.size = 0;
		if (diff->opts.flags & GIT_DIFF_FORCE_TEXT)
			diff->repo, delta, &old_data, &new_data)) < GIT_SUCCESS)
		/* TODO: if ignore_whitespace is set, then we *must* do text
		 * diffs to tell if a file has really been changed.
		 */

			error = file_cb(data, delta, (float)info.index / diff->files.length);
		info.delta = delta;
		xdl_diff(&old_data, &new_data,
			&xdiff_params, &xdiff_config, &xdiff_callback);
	git_diff_list *diff;
} diff_print_info;
	else if (mode & 0100)
		/* modes in git are not very flexible, so if this bit is set,
		 * we must be dealwith with a 100755 type of file.
		 */
	diff_print_info *pi = data;
	else if (delta->old_attr != delta->new_attr &&
		delta->old_attr != 0 && delta->new_attr != 0)
	diff_print_info pi;
	pi.diff     = diff;
static int print_oid_range(diff_print_info *pi, git_diff_delta *delta)
	diff_print_info *pi = data;
	const char *oldpfx = pi->diff->opts.src_prefix;
	const char *newpfx = pi->diff->opts.dst_prefix;
	git_buf_printf(pi->buf, "diff --git %s%s %s%s\n", oldpfx, delta->path, newpfx, newpath);
	diff_print_info *pi = data;
	diff_print_info *pi = data;
	diff_print_info pi;
	pi.diff     = diff;
	diff_output_info info;
	xpparam_t xdiff_params;
	xdemitconf_t xdiff_config;
	xdemitcb_t xdiff_callback;
	assert(repo);

	if (options && (options->flags & GIT_DIFF_REVERSE)) {
		git_blob *swap = old_blob;
		old_blob = new_blob;
		new_blob = swap;
	}
	info.diff    = NULL;
	info.delta   = &delta;
	info.cb_data = cb_data;
	info.hunk_cb = hunk_cb;
	info.line_cb = line_cb;
	setup_xdiff_options(options, &xdiff_config, &xdiff_params);
	memset(&xdiff_callback, 0, sizeof(xdiff_callback));
	xdiff_callback.outf = diff_output_cb;
	xdiff_callback.priv = &info;
	xdl_diff(&old, &new, &xdiff_params, &xdiff_config, &xdiff_callback);