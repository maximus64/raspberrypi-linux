/*
 * Driver for the LED Cube audio codec for Raspberry Pi.
 *
  * Author:	Khoa Hoang <admin@khoang.com>
 *		Copyright 2021
 *		based on code by Peter Malkin <petermalkin@google.com>
 *		Copyright 2016
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>
#include <sound/soc-dapm.h>

static const struct snd_soc_dapm_widget voicehat_dapm_widgets[] = {
	SND_SOC_DAPM_OUTPUT("Speaker"),
};

static const struct snd_soc_dapm_route voicehat_dapm_routes[] = {
	{"Speaker", NULL, "HiFi Playback"},
};

static const struct snd_soc_component_driver voicehat_component_driver = {
	.dapm_widgets = voicehat_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(voicehat_dapm_widgets),
	.dapm_routes = voicehat_dapm_routes,
	.num_dapm_routes = ARRAY_SIZE(voicehat_dapm_routes),
	.idle_bias_on		= 1,
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static struct snd_soc_dai_driver voicehat_dai = {
	.name = "ledcube-hifi",
	.capture = {
		.stream_name = "HiFi Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_48000,
		.formats = SNDRV_PCM_FMTBIT_S32_LE
	},
	.playback = {
		.stream_name	= "HiFi Playback",
		.formats	= SNDRV_PCM_FMTBIT_S16 |
					SNDRV_PCM_FMTBIT_S24 |
					SNDRV_PCM_FMTBIT_S32,
		.rates		= SNDRV_PCM_RATE_8000 |
					SNDRV_PCM_RATE_16000 |
					SNDRV_PCM_RATE_32000 |
					SNDRV_PCM_RATE_44100 |
					SNDRV_PCM_RATE_48000 |
					SNDRV_PCM_RATE_88200 |
					SNDRV_PCM_RATE_96000,
		.rate_min	= 8000,
		.rate_max	= 96000,
		.channels_min	= 1,
		.channels_max	= 2,
	},
	.symmetric_rates = 1
};

#ifdef CONFIG_OF
static const struct of_device_id ledcube_ids[] = {
		{ .compatible = "ledcube,ledcube-codec", }, {}
	};
	MODULE_DEVICE_TABLE(of, ledcube_ids);
#endif

static int ledcube_platform_probe(struct platform_device *pdev)
{
	return snd_soc_register_component(&pdev->dev,
					  &voicehat_component_driver,
					  &voicehat_dai,
					  1);
}

static int ledcube_platform_remove(struct platform_device *pdev)
{
	snd_soc_unregister_component(&pdev->dev);
	return 0;
}

static struct platform_driver ledcube_driver = {
	.driver = {
		.name = "ledcube-codec",
		.of_match_table = of_match_ptr(ledcube_ids),
	},
	.probe = ledcube_platform_probe,
	.remove = ledcube_platform_remove,
};

module_platform_driver(ledcube_driver);

MODULE_DESCRIPTION("LED Cube Hat Codec driver");
MODULE_AUTHOR("Khoa Hoang <admin@khoahoang.com>");
MODULE_LICENSE("GPL v2");
