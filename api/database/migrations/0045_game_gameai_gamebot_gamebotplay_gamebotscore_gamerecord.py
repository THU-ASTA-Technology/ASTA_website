# Generated by Django 3.1.4 on 2021-02-03 22:33

import database.models
from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion
import django.utils.timezone
import mdeditor.fields


class Migration(migrations.Migration):

    dependencies = [
        ('database', '0044_problemjudgedetail_memoryused'),
    ]

    operations = [
        migrations.CreateModel(
            name='Game',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(default='', max_length=128)),
                ('detail', mdeditor.fields.MDTextField()),
                ('judgeWeight', models.IntegerField(default=0)),
                ('contest', models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.SET_NULL, related_name='game', to='database.contest')),
            ],
        ),
        migrations.CreateModel(
            name='GameBot',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('addition', models.CharField(default='', max_length=128)),
                ('code', models.FileField(null=True, upload_to=database.models.GameBot_dirpath)),
                ('ai', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='bot', to=settings.AUTH_USER_MODEL)),
            ],
        ),
        migrations.CreateModel(
            name='GameRecord',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('timestamp', models.DateTimeField(default=django.utils.timezone.now)),
                ('timeUsed', models.FloatField(default=0)),
            ],
        ),
        migrations.CreateModel(
            name='GameBotScore',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('score', models.FloatField(default=0)),
                ('bot', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='bot', to='database.gamebot')),
            ],
        ),
        migrations.CreateModel(
            name='GameBotPlay',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('originalScore', models.FloatField(default=0)),
                ('deltaScore', models.FloatField(default=0)),
                ('bot', models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='database.gamebot')),
                ('record', models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='database.gamerecord')),
            ],
        ),
        migrations.CreateModel(
            name='GameAi',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('title', models.CharField(default='', max_length=128)),
                ('game', models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='ai', to='database.game')),
                ('team', models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='database.team')),
                ('user', models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL)),
            ],
        ),
    ]