#ifndef ASSMAN_H
#define ASSMAN_H


typedef struct AssMan AssMan;

typedef void *(*AssLoaderFn)(const char *path,  void *data);
typedef void  (*AssReleaseFn)(void      *asset, void *data);

AssMan *AssMan_new(void);
void    AssMan_free(AssMan *assman);
void   *AssMan_load(
	AssMan       *assman, 
	const char   *path, 
	AssLoaderFn   loader,   
	void         *load_data, 
	AssReleaseFn  releaser, 
	void         *release_data
);
void    AssMan_release(AssMan *assman, const char *path);
void    AssMan_clear(AssMan *assman);


#endif /* ASSMAN_H */
