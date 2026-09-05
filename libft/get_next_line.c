/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chajeon <chajeon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 19:31:24 by dancel            #+#    #+#             */
/*   Updated: 2025/02/14 17:09:41 by chajeon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*extract_line(char **stash)
{
	size_t	i;
	char	*line;
	char	*temp;

	i = 0;
	while ((*stash)[i] && (*stash)[i] != '\n')
		i++;
	line = ft_substr(*stash, 0, i + ((*stash)[i] == '\n'));
	if (!line)
		return (free(*stash), *stash = NULL, NULL);
	temp = ft_strdup(*stash + i + ((*stash)[i] == '\n'));
	free(*stash);
	*stash = temp;
	return (line);
}

char	*get_next_line_2(int fd, char *buffer, char **stash)
{
	ssize_t	bytes_read;
	char	*temp;

	if (!*stash)
		*stash = ft_strdup("");
	if (!*stash)
		return (free(buffer), NULL);
	while (!ft_strchr(*stash, '\n'))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
			return (free(*stash), free(buffer), *stash = NULL, NULL);
		if (bytes_read == 0)
			break ;
		buffer[bytes_read] = '\0';
		temp = ft_strjoin(*stash, buffer);
		if (!temp)
			return (free(*stash), free(buffer), *stash = NULL, NULL);
		free(*stash);
		*stash = temp;
	}
	if (!**stash)
		return (free(buffer), free(*stash), *stash = NULL, NULL);
	return (free(buffer), extract_line(stash));
}

char	*get_next_line(int fd)
{
	static char	*stash[FD_MAX];
	char		*buffer;

	if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0)
		return (NULL);
	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	return (get_next_line_2(fd, buffer, &stash[fd]));
}
