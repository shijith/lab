#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x14522340, "module_layout" },
	{ 0xfa2e111f, "slab_buffer_size" },
	{ 0xd691cba2, "malloc_sizes" },
	{ 0x105e2727, "__tracepoint_kmalloc" },
	{ 0x1a6d6e4f, "remove_proc_entry" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0xed113e22, "proc_mkdir" },
	{ 0xf85ccdae, "kmem_cache_alloc_notrace" },
	{ 0xb4390f9a, "mcount" },
	{ 0x6d6b15ff, "create_proc_entry" },
	{ 0x37a0cba, "kfree" },
	{ 0x3302b500, "copy_from_user" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "47C3DA291744F68080F69CB");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 3,
};
